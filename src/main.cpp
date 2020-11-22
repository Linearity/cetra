/*
WHY I WROTE THIS FILE:

I wanted a clean batch program that produces a coarse, tetrahedral volume mesh
from a finer, triangular surface mesh.

by Alex Stuart 'Linearity'
*/

#include <float.h>
#include <fstream>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <time.h>

#include "collisionqueries.h"
#include "BccLattice.h"
#include "IndexTet.h"
#include "mesh_query.h"
#include "slMatrix.H"
#include "slUtil.H"
#include "slVector.H"
#include "surftrack.h"
#include "SurfaceTracking/slcSurface.H"
#include "TetMesh.h"
#include "vec.h"

#define DEBUG 3
#define CRYSTAL_CULLING 1
#define RANDOM_OFFSET 1
#define RANDOM_ROTATION 1
#define OUTPUT_FRAMES 0

#define MESH_VOLUME_RATIO 1.1
#define CULLING_TEST_DISTANCE_RATIO 1.2
#define EL_TOPO_TIMESTEP 0.01
#define DEFAULT_OFFSET_BAND_RATIO 0.2
#define MAX_SLIVER_VOLUME 0.25
#define MAX_ITERATIONS 1000


// global sentinel for iterative smoothing
bool in_smoothing_stage = false;

// defined in LevelSetSurface.cpp
bool createLevelSetSurface(std::vector<SlVector3>& vertices, std::vector<SlTri>& triangles, SlcSurface &s, SlVector3 boundingBoxCornerMin, SlVector3 boundingBoxCornerMax, int treeLevel);

// normals vector must be empty
void computeFaceNormals(std::vector<Vec3d> vertices, std::vector<Vec3ui> triangles, std::vector<Vec3d>& normals)
{
	// we assume triangles are oriented according to the right-hand rule
	Vec3d n;
	int i;
	for (i = 0; i < triangles.size(); i++)
	{
		n = cross(vertices[triangles[i][1]] - vertices[triangles[i][0]], vertices[triangles[i][2]] - vertices[triangles[i][0]]);
		normals.push_back(n / mag(n));
	}
}

static double triangleArea(SlVector3& v1, SlVector3& v2, SlVector3& v3)
{
	// Heron's formula
	double s1 = mag(v2 - v1), s2 = mag(v3 - v2), s3 = mag(v1 - v3);
	double sp = (s1 + s2 + s3) * 0.5;
	return sqrt(sp * (sp - s1) * (sp - s2) * (sp - s3));
}

double polyhedronVolume(std::vector<Vec3d> vertices, std::vector<Vec3ui> triangles, std::vector<Vec3d>& normals)
{
	// Divergence theorem
	double retval = 0;
	SlVector3 v1, v2, v3, centroid;
	
	int i;
	for (i = 0; i < triangles.size(); i++)
	{
		v1 = SlVector3(vertices[triangles[i][0]].v);
		v2 = SlVector3(vertices[triangles[i][1]].v);
		v3 = SlVector3(vertices[triangles[i][2]].v);
		centroid = (v1 + v2 + v3) / 3.0;
		
		retval += dot(Vec3d(centroid.data), normals[i]) * triangleArea(v1, v2, v3);
	}
	
	return retval / 3.0;
}

void interrupt_handler(int signal)
{
	char answer;
	
	if (in_smoothing_stage)
	{
		printf("\nSmoothing is in progress. Save current iteration? (y/n): ");
		while (1)
		{
			answer = getc(stdin);
			if (answer == 'y')
			{
				in_smoothing_stage = false;
				// let the program finish up
				break;
			}
			else if (answer == 'n')
			{
				// quit immediately
				exit(0);
			}
			else
			{
				while (getc(stdin) != '\n');
				printf("Please enter \"y\" or \"n\": ");
			}
		}
	}
	else
	{
		printf("\n");
		// quit immediately
		exit(0);
	}
}

int main(int argc, char **argv)
{
	// check for a lack of arguments
	if (argc < 2)
	{
		printf("usage: %s [-v] [-o output_prefix] [-b offset_band_width] [-h lattice_cell_width] input_mesh\n", argv[0]);
		return -1;
	}
	
	int i, j, k, l;					// general loop indices I always use
	
	std::string trimesh_filename(argv[argc - 1]);	// name of surface mesh input file
	double h = 0;					// world-space width of lattice cells
	unsigned int nx;				// number of lattice cells along x-axis
	unsigned int ny;				// number of lattice cells along y-axis
	unsigned int nz;				// number of lattice cells along z-axis
	
	// check for flags
	bool verbose = false;
	bool cull = true;
	bool smooth = true;
	bool average_one_rings = true;
	std::string output_prefix = "awesome-tetmesh";
	double offset_band_width = DEFAULT_OFFSET_BAND_RATIO;
	for (i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-v") == 0)
		{
			verbose = true;
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			if (argv[i + 1][0] == '-')
			{
				printf("usage: %s [-v] [-o output_prefix] [-b offset_band_width] h input_mesh\n", argv[0]);
				return -1;
			}
			else
			{
				output_prefix = argv[i + 1];
				i++;
			}
		}
		else if (strcmp(argv[i], "-b") == 0)
		{
			if (argv[i + 1][0] == '-')
			{
				printf("usage: %s [-v] [-o output_prefix] [-b offset_band_width] h input_mesh\n", argv[0]);
				return -1;
			}
			else
			{
				offset_band_width = atof(argv[i + 1]);	// STILL NEEDS TO BE MULTIPLIED BY h!
				i++;
			}
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			if (argv[i + 1][0] == '-')
			{
				printf("usage: %s [-v] [-o output_prefix] [-b offset_band_width] h input_mesh\n", argv[0]);
				return -1;
			}
			else
			{
				h = atof(argv[i + 1]);
				i++;
			}
		}
		else if (strcmp(argv[i], "-noculling") == 0)
		{
			cull = false;
		}
		else if (strcmp(argv[i], "-nosmoothing") == 0)
		{
			smooth = false;
		}
		else if (strcmp(argv[i], "-noaveraging") == 0)
		{
			average_one_rings = false;
		}
	}
	
	#if DEBUG > 0
	verbose = true;
	#endif
	
	// catch Ctrl+C so we can quit without losing our work
	struct sigaction catchCtrlC;
	catchCtrlC.sa_handler = interrupt_handler;
	catchCtrlC.sa_flags = 0;
	if (	   (sigemptyset(&catchCtrlC.sa_mask) == -1)
		|| (sigaction(SIGINT, &catchCtrlC, NULL) == -1)	)
	{
		fprintf(stderr, "(installing SIGINT signal handler) error: unable to install SIGINT signal handler\n");
	}
	
	
	timeval startTime, endTime;
	timeval startTotalTime, endTotalTime;
	double sculptingTime, optimizationTime, totalTime;
	
	gettimeofday(&startTotalTime, NULL);
	
	
	// READ OBJ FILE
	std::vector<SlVector3> obj_vertices;
	std::vector<SlTri> obj_triangles;
	
	bool success = readObjFile((char *)trimesh_filename.c_str(), obj_vertices, obj_triangles);
	if (!success)
	{
		fprintf(stderr, "(opening input mesh file) error: unable to read OBJ file.\n");
		return false;
	}
	
	
	char output_postfix[80];
	
	#if RANDOM_ROTATION
	srand( time( NULL ) );
	double random1 = (double)rand() / (double)RAND_MAX, random2 = (double)rand() / (double)RAND_MAX, random3 = (double)rand() / (double)RAND_MAX;
	SlVector3 reflectionNormal( cos(2 * M_PI * random2) * sqrt(random3), sin(2 * M_PI * random2) * sqrt(random3), sqrt(1 - random3) );
	SlMatrix3x3 outerProduct( reflectionNormal[0] * reflectionNormal, reflectionNormal[1] * reflectionNormal, reflectionNormal[2] * reflectionNormal, SlMatrix3x3::col );
	SlMatrix3x3 H;
	H.setIdentity();
	H = H - 2 * outerProduct;
	SlMatrix3x3 R( cos(2 * M_PI * random1), sin(2 * M_PI * random1), 0, -sin(2 * M_PI * random1), cos(2 * M_PI * random1), 0, 0, 0, 1 );
	H = -1.0 * H * R;
	
	for ( std::vector<SlVector3>::iterator vertIter = obj_vertices.begin(); vertIter != obj_vertices.end(); vertIter++ )
	{
		*vertIter = H * (*vertIter);
	}
	
	sprintf(output_postfix, "_%1.3f", random1);
	output_prefix += output_postfix;
	#endif
	
	
	
	// COMPUTE LATTICE BOUNDING BOX FROM TRIANGLE MESH BOUNDING BOX
	SlVector3 center(0, 0, 0), cornerMin(0, 0, 0), cornerMax(0, 0, 0);
	double bb_dim_x, bb_dim_y, bb_dim_z;
	
	// compute triangle mesh bounding box
	for (std::vector<SlVector3>::iterator vertIter = obj_vertices.begin(); vertIter != obj_vertices.end(); vertIter++)
	{
		cornerMin[0] = std::min(cornerMin[0], (*vertIter)[0]);
		cornerMin[1] = std::min(cornerMin[1], (*vertIter)[1]);
		cornerMin[2] = std::min(cornerMin[2], (*vertIter)[2]);
		
		cornerMax[0] = std::max(cornerMax[0], (*vertIter)[0]);
		cornerMax[1] = std::max(cornerMax[1], (*vertIter)[1]);
		cornerMax[2] = std::max(cornerMax[2], (*vertIter)[2]);
	}
	
	bb_dim_x = cornerMax[0] - cornerMin[0];
	bb_dim_y = cornerMax[1] - cornerMin[1];
	bb_dim_z = cornerMax[2] - cornerMin[2];
	center = cornerMin + SlVector3(bb_dim_x / 2.0, bb_dim_y / 2.0, bb_dim_z / 2.0);
	
	// compute h if not set in command line arguments
	if (h == 0)
	{
		double bb_dim_min = DBL_MAX;
		if (bb_dim_x < bb_dim_min) bb_dim_min = bb_dim_x;
		if (bb_dim_y < bb_dim_min) bb_dim_min = bb_dim_y;
		if (bb_dim_z < bb_dim_min) bb_dim_min = bb_dim_z;
		h = 0.2 * bb_dim_min;
	}
	
	// update offset_band_width
	offset_band_width *= h;
	
	#if RANDOM_OFFSET
	srand( time( NULL ) );
	SlVector3 rand_bb_offset = SlVector3( ((double)rand() / (double)RAND_MAX - 0.5) * h, ((double)rand() / (double)RAND_MAX - 0.5) * h, ((double)rand() / (double)RAND_MAX - 0.5) * h );
	center += rand_bb_offset;
	
	sprintf(output_postfix, "_%1.3f_%1.3f_%1.3f", rand_bb_offset[0], rand_bb_offset[1], rand_bb_offset[2]);
	output_prefix += output_postfix;
	#endif
	
	// compute lattice bounding box
	nx = (unsigned int)(bb_dim_x / h) + 2;
	ny = (unsigned int)(bb_dim_y / h) + 2;
	nz = (unsigned int)(bb_dim_z / h) + 2;
	
	cornerMin = center + SlVector3(-(double)nx * h / 2.0, -(double)ny * h / 2.0, -(double)nz * h / 2.0);
	cornerMax = center + SlVector3((double)nx * h / 2.0, (double)ny * h / 2.0, (double)nz * h / 2.0);
	
	
	
	// CREATE BCC TETRAHEDRA
	SlVector3 *lattice_vertices = new SlVector3[countBccVertices(nx, ny, nz)];
	IndexTet *lattice_tets = new IndexTet[countBccTets(nx, ny, nz)];
	
	if (verbose) { printf("Creating BCC lattice..."); fflush(stdout); }
	generateBccLattice(nx, ny, nz, h, cornerMin, lattice_vertices, lattice_tets);
	if (verbose) { printf("done!\n"); }
	
	TetMesh tetmesh(lattice_vertices, countBccVertices(nx, ny, nz), lattice_tets, countBccTets(nx, ny, nz));
	
	#if DEBUG > 0
	fprintf(stderr, "There are %d lattice tets.\n", tetmesh.tet_count);
	#endif
	
	
	
	// CREATE LEVEL SET SURFACE FOR THE TRIANGLE MESH
	SlcSurface s;
	if (verbose) { printf("Creating level set surface..."); fflush(stdout); }
	// NOTE: the corners must be offset by a slight factor to make the distance field not fuck up
	
	SlVector3 diagonal = cornerMax - cornerMin;
	center = (cornerMin + cornerMax) / 2.0;
	double max_dim = std::max( diagonal[0], std::max( diagonal[1], diagonal[2] ) );
	cornerMin = (center - SlVector3( max_dim, max_dim, max_dim ) / 2.0) * 1.25;
	cornerMax = (center + SlVector3( max_dim, max_dim, max_dim ) / 2.0) * 1.25;
	
	if (!createLevelSetSurface(obj_vertices, obj_triangles, s, cornerMin, cornerMax, 7))
	{
		fprintf(stderr, "(creating level set from input mesh) error: unable to create level set.\n");
		return -1;
	}
	if (verbose) printf("done!\n");
	
	
	
	// COMPUTE CIRCUMSPHERES
	SlVector3 tri_circumcenters[s.triangles.size()];
	double tri_circumradii[s.triangles.size()];
	SlVector3 s1, s2, s3, c;
	double l1, l2, l3;
	
	if (verbose) { printf("Computing tetrahedron circumspheres..."); fflush(stdout); }
	tetmesh.computeCircumspheres();
	if (verbose) printf("done!\n");
	
	if (verbose) { printf("Computing triangle circumspheres..."); fflush(stdout); }
	for (std::vector<SlTri>::iterator triIter = s.triangles.begin(); triIter != s.triangles.end(); triIter++)
	{
		i = triIter - s.triangles.begin();
		s1 = s.meshPts[s.triangles[i][0]] - s.meshPts[s.triangles[i][2]];
		s2 = s.meshPts[s.triangles[i][1]] - s.meshPts[s.triangles[i][2]];
		s3 = s.meshPts[s.triangles[i][1]] - s.meshPts[s.triangles[i][0]];
		l1 = mag(s1);
		l2 = mag(s2);
		l3 = mag(s3);
		c = cross(s1, s2);
		tri_circumradii[i] = l1 * l2 * mag(s1 - s2) / (2 * mag(c));
		tri_circumcenters[i] = cross(l1 * l1 * s2 - l2 * l2 * s1, c) / (2 * sqrMag(c)) + s.meshPts[s.triangles[i][2]];
	}
	if (verbose) printf("done!\n");
	
	
	
	int num_incident_tets = 0;
	int max_num_incident_tets = 0;
	
	
	gettimeofday(&startTime, NULL);
	
	MeshObject *mq_surface;
	// CUT OUT EXTRANNEOUS TETRAHEDRA
	if (cull)
	{
		IndexTet *tet_i;
		double tet_radius, tri_radius;
		int mq_dummy_int = 0;
		double mq_dummy_dbl = 0;
		std::vector<SlTri>::iterator triIter;
		
		double surface_vertex_data[s.meshPts.size() * 3];
		for (i = 0; i < s.meshPts.size(); i++)
		{
			surface_vertex_data[i * 3] = s.meshPts[i][0];
			surface_vertex_data[i * 3 + 1] = s.meshPts[i][1];
			surface_vertex_data[i * 3 + 2] = s.meshPts[i][2];
		}
		
		int surface_triangle_data[s.triangles.size() * 3];
		for (i = 0; i < s.triangles.size(); i++)
		{
			surface_triangle_data[i * 3] = s.triangles[i][0];
			surface_triangle_data[i * 3 + 1] = s.triangles[i][1];
			surface_triangle_data[i * 3 + 2] = s.triangles[i][2];
		}
		
		mq_surface = construct_mesh_object(s.meshPts.size(), surface_vertex_data, s.triangles.size(), surface_triangle_data);
		
		if (verbose) { printf("Cutting out extranneous tetrahedra..."); fflush(stdout); }
		for (i = 0; i < tetmesh.tet_count; i++)
		{
			// tet is guilty until proven innocent
			tetmesh.markTet(i);
			tet_radius = tetmesh.getCircumradius(tetmesh.getTetPointer(i));
			
			if (	   point_inside_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[0])).data, mq_surface)
				|| point_inside_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[1])).data, mq_surface)
				|| point_inside_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[2])).data, mq_surface)
				|| point_inside_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[3])).data, mq_surface)	)
			{
				tetmesh.unmarkTet(i);
			}
			
			if (tetmesh.tetMarked(i))
			{
				for (std::vector<SlVector3>::iterator vertIter = s.meshPts.begin(); vertIter != s.meshPts.end(); vertIter++)
				{
					if (	  mag(tetmesh.getCircumcenter(tetmesh.getTetPointer(i)) - *vertIter)
						< CULLING_TEST_DISTANCE_RATIO * tet_radius	)
					{
						if ( tetmesh.pointInTet(*vertIter, tetmesh.getTetPointer(i)) )
						{
							tetmesh.unmarkTet(i);
						}
					}
				}
			}
			
			if (tetmesh.tetMarked(i))
			{       
				if (	   segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[0])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[1])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)
					|| segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[0])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[2])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)
					|| segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[0])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[3])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)
					|| segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[1])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[2])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)
					|| segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[1])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[3])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)
					|| segment_intersects_mesh((*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[2])).data, (*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[3])).data, mq_surface, &j, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl, &mq_dummy_dbl)	)
				{
					tetmesh.unmarkTet(i);
				}				
			}
			
			if (verbose && (i != 0) && ((i % 100) == 0))
			{
				printf("\rCutting out extranneous tetrahedra...                          ");
				fflush(stdout);
				printf("\rCutting out extranneous tetrahedra...%d tets checked...", i);
				fflush(stdout);
			}
		}
#if CRYSTAL_CULLING
		tetmesh.computeAdjacentTets();
		
		// count how many interior tets are incident to each vertex
		int num_interior_incident_tets[tetmesh.vertex_count];
		int incident_tets[24];
		for (i = 0; i < tetmesh.vertex_count; i++)
		{
			num_interior_incident_tets[i] = 0;
			tetmesh.computeIncidentTets(tetmesh.getVertexPointer(i));
			tetmesh.getIncidentTets(tetmesh.getVertexPointer(i), incident_tets);
			num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(i));
			if (num_incident_tets == 24)
			{
				for (j = 0; j < num_incident_tets; j++)
				{
					if (!tetmesh.tetMarked(incident_tets[j]))
					{
						num_interior_incident_tets[i]++;
					}
				}
			}
			else
			{
				// mark this as a non-candidate for an added snowflake
				num_interior_incident_tets[i] = -1;
			}
		}
		
		int max_interior_incident_tets;
		int max_j = 0;
		int num_adjacent_tets;
		int adjacent_tets[4];
		int num_marked_adjacent_tets;
		
		// for each tet with four surface vertices...
		for (i = 0; i < tetmesh.tet_count; i++)
		{
			tetmesh.getAdjacentTets(tetmesh.getTetPointer(i), adjacent_tets);
			num_adjacent_tets = tetmesh.numberOfAdjacentTets(tetmesh.getTetPointer(i));
			num_marked_adjacent_tets = 0;
			for (j = 0; j < num_adjacent_tets; j++)
			{
				if (tetmesh.tetMarked(adjacent_tets[j]))
				{
					num_marked_adjacent_tets++;
				}
			}
			if (!tetmesh.tetMarked(i) && (num_adjacent_tets - num_marked_adjacent_tets < 3))
			{
				// ...find which of its vertices has the most interior tets incident to it...
				max_interior_incident_tets = 0;
				for (j = 0; j < 4; j++)
				{
					if (num_interior_incident_tets[(*tetmesh.getTetPointer(i))[j]] > max_interior_incident_tets)
					{
						max_interior_incident_tets = num_interior_incident_tets[(*tetmesh.getTetPointer(i))[j]];
						max_j = j;
					}
				}
				
				assert( (0 <= max_j) && (max_j < 4) );
				
				// ...and unmark every tet incident to that vertex
				tetmesh.getIncidentTets(tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[max_j]), incident_tets);
				num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[max_j]));
				if (num_incident_tets != 24)
				{
					for (j = 0; j < 4; j++)
					{
						printf("%d\n", tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[j])));
					}
				}
				for (j = 0; j < num_incident_tets; j++)
				{
					tetmesh.unmarkTet(incident_tets[j]);
				}
				
				num_marked_adjacent_tets = 0;
				for (j = 0; j < num_adjacent_tets; j++)
				{
					if (tetmesh.tetMarked(adjacent_tets[j]))
					{
						num_marked_adjacent_tets++;
					}
				}
				
				assert(num_adjacent_tets - num_marked_adjacent_tets > 2);
			}
		}
#endif
		
		tetmesh.removeMarkedTets();
		
		if (verbose)
		{
			printf("\rCutting out extranneous tetrahedra...%d tets checked...done!\n", i);
		}
		
		tetmesh.clearVertexMarks();
		for (i = 0; i < tetmesh.vertex_count; i++)
		{
			tetmesh.computeIncidentTets(tetmesh.getVertexPointer(i));
			if (tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(i)) == 0)
			{
				tetmesh.markVertex(i);
			}
		}
		tetmesh.removeMarkedVertices();
		
		#if DEBUG > 0
		fprintf(stderr, "There are %d unculled tets.\n", tetmesh.tet_count);
		#endif
	}
	
	gettimeofday(&endTime, NULL);
	sculptingTime = (endTime.tv_sec - startTime.tv_sec) +
		(endTime.tv_usec-startTime.tv_usec)*1.0e-6;
	
	#if DEBUG > 2
	// check for inverted tets
	int inverted_tet_count = 0;
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		if (tetmesh.orient3d(tetmesh.getTetPointer(i)) <= 0)
		{
			inverted_tet_count++;
		}
	}
	if (inverted_tet_count > 0)
	{
		fprintf(stderr, "There are %d inverted tets before smoothing.\n", inverted_tet_count);
	}
	#endif
	
	gettimeofday(&startTime, NULL);
	
	// CREATE REPRESENTATION OF OUTER SURFACE OF LATTICE
	SlVector3 *surface_vertices = new SlVector3[tetmesh.vertex_count];
	int surface_vertex_count = 0;
	SlTri *surface_triangles = new SlTri[4 * tetmesh.tet_count];
	int surface_triangle_count = 0;
	
	if (verbose) { printf("Extracting tet mesh surface..."); fflush(stdout); }
	tetmesh.extractSurface(surface_vertices, &surface_vertex_count, surface_triangles, &surface_triangle_count);
	if (verbose) { printf("done!\n"); }
	#if DEBUG > 0
	fprintf(stderr, "There are %d surface vertices.\n", surface_vertex_count);
	fprintf(stderr, "There are %d surface triangles.\n", surface_triangle_count);
	#endif

	SurfTrack *surf;
	
	double mesh_volume, prev_mesh_volume, model_volume;
	double max_distance = 0, min_distance = DBL_MAX, avg_distance = 0;
	
	std::ofstream out_stream;
	std::ofstream out_stream_bin;
	SlTri faces[4 * tetmesh.tet_count];
	int face_count;
	
	// output_prefix + "_i" + 99999 + ".obj"
	char *iteration_filename = (char *)malloc( output_prefix.size() + 2 + 5 + 4 );
	
	double current_da;
	double min_da;
	double min_das[tetmesh.tet_count];
	
	if (smooth)
	{
		in_smoothing_stage = true;
		
		// SET UP EL TOPO
		if (verbose) { printf("Setting up El Topo...\n"); }
		std::vector<Vec3d> vs;
		std::vector<Vec3ui> ts;
		std::vector<double> masses;
		
		std::vector<Vec3d> surface_normals;
		std::vector<Vec3d> model_normals;
		
		struct SurfTrackInitializationParameters params;
		params.m_min_edge_length = 0.0;
		params.m_max_edge_length = DBL_MAX;
		params.m_max_volume_change = DBL_MAX;
		params.m_collision_safety = true;
		params.m_allow_topology_changes = false;
		params.m_perform_improvement = false;
		
		
		
		// SET UP CIRCUMCENTER AVERAGING
		SlVector3 average, difference;
		double incident_tet_volume, total_incident_tet_volume;
		double global_change = DBL_MAX;
		double local_change = 0;
		int general_j = 0;		// for converted surface indices
		
		num_incident_tets = 0;
		max_num_incident_tets = 0;
		
		for (i = 0; i < tetmesh.vertex_count; i++)
		{
			tetmesh.computeIncidentTets(tetmesh.getVertexPointer(i));
			num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(i));
			if (num_incident_tets > max_num_incident_tets)
			{
				max_num_incident_tets = num_incident_tets;
			}
		}
		
		int incident_tets[max_num_incident_tets];
		
		
		
		// add lattice surface vertices
		for (i = 0; i < surface_vertex_count; i++)
		{
			vs.push_back(Vec3d(surface_vertices[i].data));
			masses.push_back(0.01);
		}
		
		// add lattice surface triangles
		for (i = 0; i < surface_triangle_count; i++)
		{
			ts.push_back(Vec3ui(surface_triangles[i].indices));
		}
		
		// add model surface vertices
		for (std::vector<SlVector3>::iterator vertIter = s.meshPts.begin(); vertIter != s.meshPts.end(); vertIter++)
		{
			vs.push_back(Vec3d(vertIter->data));
			masses.push_back(1000000);
		}
		
		// add model surface triangles
		for (std::vector<SlTri>::iterator triIter = s.triangles.begin(); triIter != s.triangles.end(); triIter++)
		{
			ts.push_back(Vec3ui(surface_vertex_count + triIter->indices[0], surface_vertex_count + triIter->indices[1], surface_vertex_count + triIter->indices[2]));
		}
		
		surf = new SurfTrack(vs, ts, masses, params);
		if (verbose) { printf("Setting up El Topo...done!\n"); }
		
		// compute normals
		surface_normals.clear();
		model_normals.clear();
		computeFaceNormals(surf->m_positions, surf->m_mesh.m_tris, surface_normals);
		model_normals = std::vector<Vec3d>(surface_normals.begin() + surface_triangle_count, surface_normals.end());
		surface_normals = std::vector<Vec3d>(surface_normals.begin(), surface_normals.begin() + surface_triangle_count);


		
		// RUN ITERATIVE OPTIMIZATIONS UNTIL MESH VOLUME APPROXIMATES MODEL VOLUME
		IndexTet *smallest_incident_tet;
		int incident_vertex;
		int outside_vertex_count = INT_MAX;
		//SlVector3 grad;
		i = 0;
		
		model_volume = polyhedronVolume(surf->m_positions, std::vector<Vec3ui>(surf->m_mesh.m_tris.begin() + surface_triangle_count, surf->m_mesh.m_tris.end()), model_normals);
		mesh_volume = DBL_MAX;
		
		SlVector3 expected_surface_vertices[surface_vertex_count];
		memset(expected_surface_vertices, 0, surface_vertex_count);
		
		if (verbose) { printf("Running El Topo's collision-free advection...\n"); fflush(stdout); }
		// *** epsilon parameter here ***
		while (	   in_smoothing_stage
			&& /*((	  (mesh_volume = polyhedronVolume(surf->m_positions, std::vector<Vec3ui>(surf->m_mesh.m_tris.begin(), surf->m_mesh.m_tris.begin() + surface_triangle_count), surface_normals))
				/ (model_volume) ) > MESH_VOLUME_RATIO)	)*/
			   /* global_change > 0.01) */
			   outside_vertex_count > 0)
		{
			/* DO NOT OVERWRITE i IN THIS BLOCK! */
			
			prev_mesh_volume = mesh_volume;
			mesh_volume = polyhedronVolume(surf->m_positions, std::vector<Vec3ui>(surf->m_mesh.m_tris.begin(), surf->m_mesh.m_tris.begin() + surface_triangle_count), surface_normals);
			
			#if DEBUG > 2
			assert(mesh_volume > 0);
			assert(model_volume > 0);
			#endif
			
			#if DEBUG > 1                        
			fprintf(stderr, "Mesh volume is %f.\n", mesh_volume);
			fprintf(stderr, "Model volume is %f.\n", model_volume);
			#endif
			
			
			
			// STEP 1: MINIMIZE DISTANCE TO SURFACE
			
			tetmesh.computeCircumspheres();
			
			surf->m_velocities.clear();
			
			global_change = 0.0;
			
			memset(expected_surface_vertices, 0, surface_vertex_count);
			
			// for all surface vertices
			for (j = 0; j < surface_vertex_count; j++)
			{
				general_j = tetmesh.surface2AllIndex(j);
				
				/*
				grad = SlVector3(	(   s.eval(*tetmesh.getVertexPointer(general_j) + SlVector3(0.1 * h, 0, 0))
							  - s.eval(*tetmesh.getVertexPointer(general_j) - SlVector3(0.1 * h, 0, 0)) ) / (0.2 * h),
							
							(   s.eval(*tetmesh.getVertexPointer(general_j) + SlVector3(0, 0.1 * h, 0))
							  - s.eval(*tetmesh.getVertexPointer(general_j) - SlVector3(0, 0.1 * h, 0)) ) / (0.2 * h),
							
							(   s.eval(*tetmesh.getVertexPointer(general_j) + SlVector3(0, 0, 0.1 * h))
							  - s.eval(*tetmesh.getVertexPointer(general_j) - SlVector3(0, 0, 0.1 * h)) ) / (0.2 * h)	);*/
				
				//grad = s.grad(*tetmesh.getVertexPointer(general_j));
				
				average = SlVector3(0, 0, 0);
				tetmesh.getIncidentTets(tetmesh.getVertexPointer(general_j), incident_tets);
				num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(general_j));
				
				if (num_incident_tets > 0)
				{
					total_incident_tet_volume = 0;
					
					for (k = 0; k < num_incident_tets; k++)
					{
						incident_tet_volume = tetmesh.getVolume(tetmesh.getTetPointer(incident_tets[k]));
						total_incident_tet_volume += incident_tet_volume;
						average += incident_tet_volume * tetmesh.getCircumcenter(tetmesh.getTetPointer(incident_tets[k]));
					}
									
					average /= total_incident_tet_volume;
					difference = average - *tetmesh.getVertexPointer(general_j);
					
					/*
					if (s.eval(*tetmesh.getVertexPointer(general_j)) < offset_band_width)
					{
						difference = std::max(dot(difference, grad), 0.0) / sqrMag(grad) * grad;
					}
					*/
					
					surf->m_velocities.push_back(Vec3d(difference.data));
				}
				
				expected_surface_vertices[j] = *tetmesh.getVertexPointer(general_j) + difference * EL_TOPO_TIMESTEP;
			}
			
			// for all non-surface vertices
			for (j = 0; j < tetmesh.vertex_count; j++)
			{
				if (!tetmesh.isSurfaceVertex(j))
				{
					average = SlVector3(0, 0, 0);
					tetmesh.getIncidentTets(tetmesh.getVertexPointer(j), incident_tets);
					num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(j));
					
					if (num_incident_tets > 0)
					{
						total_incident_tet_volume = 0;
						
						for (k = 0; k < num_incident_tets; k++)
						{
							incident_tet_volume = tetmesh.getVolume(tetmesh.getTetPointer(incident_tets[k]));
							total_incident_tet_volume += incident_tet_volume;
							average += incident_tet_volume * tetmesh.getCircumcenter(tetmesh.getTetPointer(incident_tets[k]));
						}
						
						average /= total_incident_tet_volume;
						difference = average - *tetmesh.getVertexPointer(j);
						
						*tetmesh.getVertexPointer(j) += EL_TOPO_TIMESTEP * difference;
					}
				}
			}
			
			for (std::vector<SlVector3>::iterator vertIter = s.meshPts.begin(); vertIter != s.meshPts.end(); vertIter++)
			{
				surf->m_velocities.push_back(Vec3d(0, 0, 0));
			}
			
			// *** epsilon parameter here ***
			surf->integrate(EL_TOPO_TIMESTEP);
			
			/*
			SlVector3 bounced_vertex;
			for (j = 0; j < surface_vertex_count; j++)
			{
				if (mag(SlVector3(surf->m_positions[j].v) - expected_surface_vertices[j]) > 0.001)
				{
					//bounced_vertex = s.grad(*tetmesh.getVertexPointer(tetmesh.surface2AllIndex(j)));
					//assert(mag(bounced_vertex) > 0);
					//bounced_vertex *= mag(SlVector3(surf->m_positions[j].v) - expected_surface_vertices[j]) / mag(bounced_vertex);
					
					bounced_vertex = 0.9 * (SlVector3(surf->m_positions[j].v) - expected_surface_vertices[j]);
					surf->m_positions[j] += Vec3d(bounced_vertex.data);
				}
			}
			*/
			
			// compute normals
			surface_normals.clear();
			model_normals.clear();
			computeFaceNormals(surf->m_positions, surf->m_mesh.m_tris, surface_normals);
			model_normals = std::vector<Vec3d>(surface_normals.begin() + surface_triangle_count, surface_normals.end());
			surface_normals = std::vector<Vec3d>(surface_normals.begin(), surface_normals.begin() + surface_triangle_count);
			
			if (verbose) { printf("...iteration %d complete...\n", i);}		
			
			// UPDATE OUTER SURFACE OF LATTICE
			if (verbose) { printf("Updating tet mesh surface..."); fflush(stdout); }
			
			for (j = 0; j < surface_vertex_count; j++)
			{
				local_change = mag(SlVector3(surf->m_positions[j].v) - surface_vertices[j]);
				if (global_change < local_change)
					global_change = local_change;
				surface_vertices[j] = SlVector3(surf->m_positions[j].v);
			}
			
			tetmesh.updateSurface(surface_vertices, surface_vertex_count, surface_triangles, surface_triangle_count);
			if (verbose) { printf("done!\n"); }
			
			#if DEBUG > 1
			fprintf(stderr, "Max change from last iteration: %f\n", global_change);
			#endif
			
			outside_vertex_count = 0;
			for (j = 0; j < tetmesh.vertex_count; j++)
			{
				if ( s.eval(*tetmesh.getVertexPointer(j)) > offset_band_width - 0.005 )
				{
					outside_vertex_count++;
				}
			}
			
			#if OUTPUT_FRAMES
			// drop a frame
			
			sprintf(iteration_filename, "%s_i%05d.obj", output_prefix.c_str(), i);
			out_stream.open(iteration_filename);
			
			for (j = 0; j < tetmesh.vertex_count; j++)
			{
				out_stream << "v " << tetmesh.vertex_array[j][0] << " " << tetmesh.vertex_array[j][1] << " " << tetmesh.vertex_array[j][2] << std::endl;
			}
			
			tetmesh.extractFaces(faces, &face_count);	
			
			for (j = 0; j < face_count; j++)
			{
				out_stream << "f " << faces[j][0] + 1 << " " << faces[j][1] + 1 << " " << faces[j][2] + 1 << std::endl;
			}
			out_stream.close();
			
			
			// write medit mesh file
			sprintf(iteration_filename, "%s.%d.mesh", output_prefix.c_str(), i);
			out_stream.open(iteration_filename);
			
			out_stream << "MeshVersionFormatted 1" << std::endl;
			out_stream << "Dimension" << std::endl;
			out_stream << "3" << std::endl;
			
			out_stream << std::endl << "Vertices" << std::endl;
			out_stream << tetmesh.vertex_count << std::endl;
			for (j = 0; j < tetmesh.vertex_count; j++)
			{
				out_stream << (*tetmesh.getVertexPointer(j))[0] << " " << (*tetmesh.getVertexPointer(j))[1] << " " << (*tetmesh.getVertexPointer(j))[2] << " 0" << std::endl;
			}
			
			out_stream << std::endl << "Tetrahedra" << std::endl;
			out_stream << tetmesh.tet_count + 2 << std::endl;
			for (j = 0; j < tetmesh.tet_count; j++)
			{
				out_stream << (*tetmesh.getTetPointer(j))[0] + 1 << " " << (*tetmesh.getTetPointer(j))[1] + 1 << " " << (*tetmesh.getTetPointer(j))[2] + 1 << " " << (*tetmesh.getTetPointer(j))[3] + 1 << " 0" << std::endl;
			}
			out_stream << "1 1 1 1 0" << std::endl;
			out_stream << "2 2 2 2 0" << std::endl;
			
			out_stream << std::endl << "End" << std::endl;
			
			out_stream.close();
			
			for (j = 0; j < tetmesh.tet_count; j++)
			{
				min_das[j] = tetmesh.getMinDihedralAngle(tetmesh.getTetPointer(j));
			}
			
			sprintf(iteration_filename, "%s.%d.bb", output_prefix.c_str(), i);
			out_stream.open(iteration_filename);
			out_stream << "3 1 " << tetmesh.tet_count + 2 << " 1" << std::endl;
				
			for (j = 0; j < tetmesh.tet_count; j++)
			{
				out_stream << min_das[j] / (2 * M_PI) * 360 << " ";
			}
			out_stream << "0 90 ";
			
			out_stream.close();
			if (verbose) { printf("done!\n"); }
			
			#endif // (OUTPUT_FRAMES)
			
			i++;
			
			if (i > MAX_ITERATIONS)
			{
				in_smoothing_stage = false;
			}
		}
		if (verbose) { printf("Running El Topo's collision-free advection...done!\n"); }
		
		#if DEBUG > 1
		fprintf(stderr, "Mesh volume is %f.\n", mesh_volume);
		fprintf(stderr, "Model volume is %f.\n", model_volume);
		#endif
		
		/*

		int outer_vertices[2];
		int inner_vertices[2];
		int outer_faces[2];
		int outer_vertex_index, inner_vertex_index, outer_face_index;
		SlVector3 pseudonormal;
		
		surf->m_velocities.assign(surf->m_velocities.size(), Vec3d(0, 0, 0));
		
		for (i = 0; i < tetmesh.tet_count; i++)
		{
			// if this tet has all four vertices on the surface
			if (	   tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[0])
				&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[1])
				&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[2])
				&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[3])
				&& (tetmesh.getVolume(tetmesh.getTetPointer(i)) < MAX_SLIVER_VOLUME * h * h * h / 12.0)	)
			{
				outer_face_index = 0;
				outer_vertex_index = 0;
				inner_vertex_index = 0;
				
				// find the outer edge
				for (j = 0; j < 4; j++)
				{
					if (tetmesh.isSurfaceFace(SlTri((*tetmesh.getTetPointer(i))[j], (*tetmesh.getTetPointer(i))[(j + 1) % 4], (*tetmesh.getTetPointer(i))[(j + 2) % 4])))
					{
						outer_faces[outer_face_index] = j;
						outer_face_index++;
					}
				}
				
				for (j = 0; j < 3; j++)
				{
					for (k = 0; k < 3; k++)
					{
						if (((outer_faces[0] + j) % 4) == ((outer_faces[1] + k) % 4))
						{
							outer_vertices[outer_vertex_index] = (outer_faces[0] + j) % 4;
							outer_vertex_index++;
						}
					}
				}
				
				for (j = 0; j < 4; j++)
				{
					if ((j != outer_vertices[0]) && (j != outer_vertices[1]))
					{
						inner_vertices[inner_vertex_index] = j;
						inner_vertex_index++;
					}
				}
				
				// find the outer edge's pseudo-normal
				pseudonormal = tetmesh.getPseudoNormal(tetmesh.getTetPointer(i), outer_vertices[0], outer_vertices[1]);
				
				// move the outer vertices in the direction of the pseudo-normal
				/*
				*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[0]]) += (OFFSET_BAND - s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[0]]))) * pseudonormal;
				*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[1]]) += (OFFSET_BAND - s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[1]]))) * pseudonormal;
				*~/

				surf->m_velocities[tetmesh.all2SurfaceIndex((*tetmesh.getTetPointer(i))[outer_vertices[0]])] += Vec3d(((OFFSET_BAND - s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[0]]))) * pseudonormal).data);
				surf->m_velocities[tetmesh.all2SurfaceIndex((*tetmesh.getTetPointer(i))[outer_vertices[1]])] += Vec3d(((OFFSET_BAND - s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[outer_vertices[1]]))) * pseudonormal).data);
				
				/*
				// find the inner edge's pseudo-normal
				pseudonormal = tetmesh.getPseudoNormal(tetmesh.getTetPointer(i), inner_vertices[0], inner_vertices[1]);
				
				// move the inner vertices in the direction of the pseudo-normal
				/*
				*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[0]]) += (0.9 * s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[0]]))) * pseudonormal;
				*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[1]]) += (0.9 * s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[1]]))) * pseudonormal;
				*~/
				
				surf->m_velocities[(*tetmesh.getTetPointer(i))[inner_vertices[0]]] = Vec3d(((0.9 * s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[0]]))) * pseudonormal).data);
				surf->m_velocities[(*tetmesh.getTetPointer(i))[inner_vertices[1]]] = Vec3d(((0.9 * s.eval(*tetmesh.getVertexPointer((*tetmesh.getTetPointer(i))[inner_vertices[1]]))) * pseudonormal).data);
				
			}
		}
		
		surf->integrate(1.0);
		
		// UPDATE OUTER SURFACE OF LATTICE
		if (verbose) { printf("Updating tet mesh surface..."); fflush(stdout); }
		
		for (j = 0; j < surface_vertex_count; j++)
		{				
			surface_vertices[j] = SlVector3(surf->m_positions[j].v);
		}
		
		tetmesh.updateSurface(surface_vertices, surface_vertex_count, surface_triangles, surface_triangle_count);
		if (verbose) { printf("done!\n"); }
		
		*/
		/*
		if (verbose) { printf("Optimizing internal vertices..."); fflush(stdout);}
		global_change = DBL_MAX;
		while (global_change > 0.0000001)
		{
			global_change = 0.0;
			
			// for all non-surface vertices
			for (j = 0; j < tetmesh.vertex_count; j++)
			{
				if (!tetmesh.isSurfaceVertex(j))
				{
					average = SlVector3(0, 0, 0);
					tetmesh.getIncidentTets(tetmesh.getVertexPointer(j), incident_tets);
					num_incident_tets = tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(j));
					
					if (num_incident_tets > 0)
					{
						total_incident_tet_volume = 0;
						
						for (k = 0; k < num_incident_tets; k++)
						{
							incident_tet_volume = tetmesh.getVolume(tetmesh.getTetPointer(incident_tets[k]));
							total_incident_tet_volume += incident_tet_volume;
							average += incident_tet_volume * tetmesh.getCircumcenter(tetmesh.getTetPointer(incident_tets[k]));
						}
						
						average /= total_incident_tet_volume;
						difference = average - *tetmesh.getVertexPointer(j);
						
						local_change = mag(EL_TOPO_TIMESTEP * difference);
						if (global_change < local_change)
							global_change = local_change;
						
						*tetmesh.getVertexPointer(j) += EL_TOPO_TIMESTEP * difference;
					}
				}
			}
			
			#if DEBUG > 1
			fprintf(stderr, "Max change from last iteration: %f\n", global_change);
			#endif
		}
		if (verbose) { printf("done!\n"); }
		*/
		in_smoothing_stage = false;
	}
	
	gettimeofday(&endTime, NULL);
	optimizationTime = (endTime.tv_sec - startTime.tv_sec) +
		(endTime.tv_usec-startTime.tv_usec)*1.0e-6;
		
	gettimeofday(&endTotalTime, NULL);
	totalTime = (endTotalTime.tv_sec - startTotalTime.tv_sec) +
		(endTotalTime.tv_usec-startTotalTime.tv_usec)*1.0e-6;
	
	#if DEBUG > 3
	// check for and record vertices that are outside band
	int outside_vertex_count = 0;
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		if (s.eval(*tetmesh.getVertexPointer(i)) > offset_band_width)
		{
			outside_vertex_count++;
		}
	}
	if (outside_vertex_count > 0)
	{
		fprintf(stderr, "There are %d outside vertices after smoothing.\n", outside_vertex_count);
	}
	#endif
	
	
	
	#if RANDOM_ROTATION
	H = inverse(H);
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		tetmesh.vertex_array[i] = H * tetmesh.vertex_array[i];
	}
	
	for (i = 0; i < s.meshPts.size(); i++)
	{
		s.meshPts[i] =  H * s.meshPts[i];
	}
	
	for (i = 0; i < surface_vertex_count; i++)
	{
		surface_vertices[i] = H * surface_vertices[i];
	}
	#endif
	
	
	// WRITE TET MESH DATA TO FILES
	int count;
	
	// write nodes file
	if (verbose) { printf("Writing nodes file..."); fflush(stdout); }
	out_stream.open((output_prefix + ".node").c_str());
	out_stream << tetmesh.vertex_count << " " << 3 << " " << 0 << " " << 0 << std::endl;
	
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << i + 1 << " " << (*tetmesh.getVertexPointer(i))[0] << " " << (*tetmesh.getVertexPointer(i))[1] << " " << (*tetmesh.getVertexPointer(i))[2] << std::endl;
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	
	// write elements file
	if (verbose) { printf("Writing elements file..."); fflush(stdout); }
	out_stream.open((output_prefix + ".ele").c_str());
	out_stream << tetmesh.tet_count << " " << 4 << " " << 0 << " " << std::endl;
	
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		out_stream << i + 1 << " " << (*tetmesh.getTetPointer(i))[0] + 1 << " " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[2] + 1 << " " << (*tetmesh.getTetPointer(i))[3] + 1 << std::endl;
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	
	// write medit mesh file
	if (verbose) { printf("Writing medit mesh file..."); fflush(stdout); }
	out_stream.open((output_prefix + ".mesh").c_str());
	
	out_stream << "MeshVersionFormatted 1" << std::endl;
	out_stream << "Dimension" << std::endl;
	out_stream << "3" << std::endl;
	
	out_stream << std::endl << "Vertices" << std::endl;
	out_stream << tetmesh.vertex_count << std::endl;
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << (*tetmesh.getVertexPointer(i))[0] << " " << (*tetmesh.getVertexPointer(i))[1] << " " << (*tetmesh.getVertexPointer(i))[2] << " 0" << std::endl;
	}
	
	out_stream << std::endl << "Tetrahedra" << std::endl;
	out_stream << tetmesh.tet_count << std::endl;
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		out_stream << (*tetmesh.getTetPointer(i))[0] + 1 << " " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[2] + 1 << " " << (*tetmesh.getTetPointer(i))[3] + 1 << " 0" << std::endl;
	}
	
	out_stream << std::endl << "End" << std::endl;
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	
	// write OBJ file
	if (verbose) { printf("Writing obj file..."); fflush(stdout); }
	out_stream.open((output_prefix + ".obj").c_str());
	
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << "v " << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
	}
	
	tetmesh.extractFaces(faces, &face_count);	
	
	for (i = 0; i < face_count; i++)
	{
		out_stream << "f " << faces[i][0] + 1 << " " << faces[i][1] + 1 << " " << faces[i][2] + 1 << std::endl;
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	
	// write surface OBJ file
	if (verbose) { printf("Writing surface obj file..."); fflush(stdout); }
	out_stream.open((output_prefix + "_surface.obj").c_str());
	
	for (i = 0; i < surface_vertex_count; i++)
	{
		out_stream << "v " << surface_vertices[i][0] << " " << surface_vertices[i][1] << " " << surface_vertices[i][2] << std::endl;
	}
	
	for (i = 0; i < surface_triangle_count; i++)
	{
		out_stream << "f " << surface_triangles[i][0] + 1 << " " << surface_triangles[i][1] + 1 << " " << surface_triangles[i][2] + 1 << std::endl;
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	
	#if DEBUG > 2
	// write the supposed model surface to check if El Topo fucked it up
	if (smooth)
	{
		if (verbose) { printf("Writing possibly perturbed original model obj file..."); fflush(stdout); }
		out_stream.open((output_prefix + "_model_surface.obj").c_str());
		for (i = surface_vertex_count; i < surface_vertex_count + s.meshPts.size(); i++)
		{
			out_stream << "v " << surf->m_positions[i][0] << " " << surf->m_positions[i][1] << " " << surf->m_positions[i][2] << std::endl;
		}
		
		for (i = surface_triangle_count; i < surface_triangle_count + s.triangles.size(); i++)
		{
			out_stream << "f " << surf->m_mesh.m_tris[i][0] - surface_vertex_count + 1 << " " << surf->m_mesh.m_tris[i][1] - surface_vertex_count + 1 << " " << surf->m_mesh.m_tris[i][2] - surface_vertex_count + 1 << std::endl;
		}
		out_stream.close();
		if (verbose) { printf("done!\n"); }
	}
	#endif
	
	#if DEBUG > 2
	// check for and record inverted tets
	inverted_tet_count = 0;
	tetmesh.clearTetMarks();
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		if (tetmesh.orient3d(tetmesh.getTetPointer(i)) <= 0)
		{
			inverted_tet_count++;
			tetmesh.markTet(i);
		}
	}
	if (inverted_tet_count > 0)
	{
		fprintf(stderr, "There are %d inverted tets after smoothing.\n", inverted_tet_count);
		fprintf(stderr, "Writing inverted tets file...");
		if (verbose) { out_stream.open((output_prefix + "_inverted_tets.obj").c_str()); fflush(stdout); }
		for (i = 0; i < tetmesh.vertex_count; i++)
		{
			out_stream << "v " << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
		}
		
		for (i = 0; i < tetmesh.tet_count; i++)
		{
			if (tetmesh.tetMarked(i))
			{
				for (j = 0; j < 4; j++)
				{
					if (j % 2 == 0)
					{
						out_stream << "f " << (*tetmesh.getTetPointer(i))[j] + 1 << " " << (*tetmesh.getTetPointer(i))[(j + 2) % 4] + 1 << " " << (*tetmesh.getTetPointer(i))[(j + 1) % 4] + 1 << std::endl;
					}
					else
					{
						out_stream << "f " << (*tetmesh.getTetPointer(i))[j] + 1 << " " << (*tetmesh.getTetPointer(i))[(j + 1) % 4] + 1 << " " << (*tetmesh.getTetPointer(i))[(j + 2) % 4] + 1 << std::endl;
					}
				}
			}
		}
		out_stream.close();
		if (verbose) { printf("done!\n"); }
	}
	#endif
	
	#if DEBUG > 2
	// calculate dihedral angles
	double max_da = 0;
	
	double max_das[tetmesh.tet_count];
	
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		max_da = 0;
		min_da = DBL_MAX;
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 0, 1);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 0, 2);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 0, 3);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 1, 2);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 1, 3);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		current_da = tetmesh.getDihedralAngle(tetmesh.getTetPointer(i), 2, 3);
		max_da = ((current_da > max_da) ? (current_da) : max_da);
		min_da = ((current_da < min_da) ? (current_da) : min_da);
		
		max_das[i] = max_da;
		min_das[i] = min_da;
	}
	
	// write the lowest-quality tet out
	max_da = 0;
	min_da = DBL_MAX;
	int max_da_i = 0, min_da_i = 0;
	
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		if (max_da < max_das[i])
		{
			max_da = max_das[i];
			max_da_i = i;
		}
		
		if (min_da > min_das[i])
		{
			min_da = min_das[i];
			min_da_i = i;
		}
	}
	
	fprintf(stderr, "Minimum dihedral angle: %f (%f degrees)\n", min_da, min_da / (2.0 * M_PI) * 360);
	fprintf(stderr, "Maximum dihedral angle: %f (%f degrees)\n", max_da, max_da / (2.0 * M_PI) * 360);
	
	out_stream.open((output_prefix + "_max-da.obj").c_str());
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << "v " << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
	}
	
	for (i = 0; i < 4; i++)
	{
		if (i % 2 == 0)
		{
			out_stream << "f " << (*tetmesh.getTetPointer(max_da_i))[i] + 1 << " " << (*tetmesh.getTetPointer(max_da_i))[(i + 1) % 4] + 1 << " " << (*tetmesh.getTetPointer(max_da_i))[(i + 2) % 4] + 1 << std::endl;
		}
		else
		{
			out_stream << "f " << (*tetmesh.getTetPointer(max_da_i))[i] + 1 << " " << (*tetmesh.getTetPointer(max_da_i))[(i + 2) % 4] + 1 << " " << (*tetmesh.getTetPointer(max_da_i))[(i + 1) % 4] + 1 << std::endl;
		}
	}
	out_stream.close();
	
	out_stream.open((output_prefix + "_min-da.obj").c_str());
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << "v " << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
	}
	
	for (i = 0; i < 4; i++)
	{
		if (i % 2 == 0)
		{
			out_stream << "f " << (*tetmesh.getTetPointer(min_da_i))[i] + 1 << " " << (*tetmesh.getTetPointer(min_da_i))[(i + 1) % 4] + 1 << " " << (*tetmesh.getTetPointer(min_da_i))[(i + 2) % 4] + 1 << std::endl;
		}
		else
		{
			out_stream << "f " << (*tetmesh.getTetPointer(min_da_i))[i] + 1 << " " << (*tetmesh.getTetPointer(min_da_i))[(i + 2) % 4] + 1 << " " << (*tetmesh.getTetPointer(min_da_i))[(i + 1) % 4] + 1 << std::endl;
		}
	}
	out_stream.close();
	
	// write out tet-quality-colored surface OFF file
	if (verbose) { printf("Writing quality-colored OFF file..."); fflush(stdout); }
	out_stream.open((output_prefix + "_quality_color.off").c_str());
	out_stream << "OFF" << std::endl << tetmesh.vertex_count << " " << surface_triangle_count << " " << 3 * surface_triangle_count << " " << std::endl << std::endl;
	
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
	}
	out_stream << std::endl;
	
	int *tets_incident_0, *tets_incident_1, *tets_incident_2;
	
	for (i = 0; i < surface_triangle_count; i++)
	{
		out_stream << "3 " << tetmesh.surface2AllIndex(surface_triangles[i][0]) << " " << tetmesh.surface2AllIndex(surface_triangles[i][1]) << " " << tetmesh.surface2AllIndex(surface_triangles[i][2]) << " ";
		
		assert((tets_incident_0 = (int *)malloc(tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][0]))) * sizeof(int))) != NULL);
		//tets_incident_0 = new int[tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(surface_triangles[i][0]))];
		tetmesh.getIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][0])), tets_incident_0);
		
		assert((tets_incident_1 = (int *)malloc(tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][1]))) * sizeof(int))) != NULL);
		//tets_incident_1 = new int[tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(surface_triangles[i][1]))];
		tetmesh.getIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][1])), tets_incident_1);
		
		assert((tets_incident_2 = (int *)malloc(tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][2]))) * sizeof(int))) != NULL);
		//tets_incident_2 = new int[tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(surface_triangles[i][2]))];
		tetmesh.getIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][2])), tets_incident_2);
		
		for (j = 0; j < tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][0]))); j++)
		{
			for (k = 0; k < tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][1]))); k++)
			{
				for (l = 0; l < tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][2]))); l++)
				{
					if ((tets_incident_0[j] == tets_incident_1[k]) && (tets_incident_1[k] == tets_incident_2[l]))
					{
						out_stream << 1.0 - min_das[tets_incident_0[j]] / (2 * M_PI) << " 0.0 " << min_das[tets_incident_0[j]] / (2 * M_PI) << std::endl;
						break;
					}
				}
				
				if (l < tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][2]))))
				{
					break;
				}
			}
			
			if (k < tetmesh.numberOfIncidentTets(tetmesh.getVertexPointer(tetmesh.surface2AllIndex(surface_triangles[i][1]))))
			{
				break;
			}
		}
		
		free(tets_incident_0);
		free(tets_incident_1);
		free(tets_incident_2);
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	#endif	
	
	#if DEBUG > 2
	if (verbose) { printf("Writing medit bb file..."); fflush(stdout); }
	out_stream.open((output_prefix + ".bb").c_str());
	out_stream << "3 1 " << tetmesh.tet_count << " 1" << std::endl;
		
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		out_stream << min_das[i] / (2 * M_PI) * 360 << " ";
	}
	
	out_stream.close();
	if (verbose) { printf("done!\n"); }
	#endif
	
	out_stream.open((output_prefix + "_all-surface.obj").c_str());
	for (i = 0; i < tetmesh.vertex_count; i++)
	{
		out_stream << "v " << tetmesh.vertex_array[i][0] << " " << tetmesh.vertex_array[i][1] << " " << tetmesh.vertex_array[i][2] << std::endl;
	}
	
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		// if this tet has all four vertices on the surface
		if (	   tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[0])
			&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[1])
			&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[2])
			&& tetmesh.isSurfaceVertex((*tetmesh.getTetPointer(i))[3])	)
		{
			out_stream << "f " << (*tetmesh.getTetPointer(i))[0] + 1 << " " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[2] + 1 << std::endl;
			out_stream << "f " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[3] + 1 << " " << (*tetmesh.getTetPointer(i))[2] + 1 << std::endl;
			out_stream << "f " << (*tetmesh.getTetPointer(i))[2] + 1 << " " << (*tetmesh.getTetPointer(i))[3] + 1 << " " << (*tetmesh.getTetPointer(i))[0] + 1 << std::endl;
			out_stream << "f " << (*tetmesh.getTetPointer(i))[3] + 1 << " " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[0] + 1 << std::endl;
		}
	}
	out_stream.close();
	
	double temp_temp_distance;
	double temp_distance;
	for (i = 0; i < surface_vertex_count; i++)
	{
		//assert( !point_inside_mesh((*tetmesh.getVertexPointer(tetmesh.surface2AllIndex(i))).data, mq_surface) );
		//temp_distance = s.eval(*tetmesh.getVertexPointer(tetmesh.surface2AllIndex(i)));
		temp_distance = DBL_MAX;
		for (j = 0; j < s.triangles.size(); j++)
		{
			check_point_triangle_proximity(Vec3d((*tetmesh.getVertexPointer(tetmesh.surface2AllIndex(i))).data), Vec3d(s.meshPts[s.triangles[j][0]].data), Vec3d(s.meshPts[s.triangles[j][1]].data), Vec3d(s.meshPts[s.triangles[j][2]].data), temp_temp_distance);
			if (temp_temp_distance < temp_distance)
			{
				temp_distance = temp_temp_distance;
			}
		}
		
		if (temp_distance < min_distance)
		{
			min_distance = temp_distance;
		}
		
		if (temp_distance > max_distance)
		{
			max_distance = temp_distance;
		}
		
		avg_distance += temp_distance;
	}
	
	avg_distance /= tetmesh.vertex_count;

	out_stream.open((output_prefix + "_metrics.txt").c_str());
	out_stream << "BCC cell width:			" << h << std::endl;
	out_stream << "Number of tets:			" << tetmesh.tet_count << std::endl;
	out_stream << "Offset:				" << offset_band_width << std::endl;
	out_stream << "Surface mesh volume:		" << model_volume << std::endl;
	out_stream << "Volume mesh volume:		" << mesh_volume << std::endl;
	out_stream << "Minimum distance to surface:	" << min_distance << std::endl;
	out_stream << "Maximum distance to surface:	" << max_distance << std::endl;
	out_stream << "Mean distance to surface:	" << avg_distance << std::endl;
	out_stream << "Minimum dihedral angle:		" << min_da << std::endl;
	out_stream << "Maximum dihedral angle:		" << max_da << std::endl;
	out_stream << "Sculpting time:			" << sculptingTime << std::endl;
	out_stream << "Optimization time:		" << optimizationTime << std::endl;
	out_stream << "Total time:			" << totalTime << std::endl;
	out_stream.close();
	
	/*
	delete surf;
	delete[] surface_triangles;	
	delete[] surface_vertices;
	delete[] lattice_tets;
	delete[] lattice_vertices;
	*/
}

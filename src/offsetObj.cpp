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
#include <time.h>

#include "slUtil.H"
#include "slVector.H"
#include "SurfaceTracking/slcSurface.H"

#define DEBUG 3

// defined in LevelSetSurface.cpp
bool createLevelSetSurface(std::vector<SlVector3>& vertices, std::vector<SlTri>& triangles, SlcSurface &s, SlVector3 boundingBoxCornerMin, SlVector3 boundingBoxCornerMax, int treeLevel);

double g_offset_distance;

SlcSurface g_s;

double g_evalPhi( const SlVector3 &x, void *data )
{
	return g_s.eval( x ) - g_offset_distance;
}

int main(int argc, char **argv)
{	
	// check for a lack of arguments
	if (argc < 4)
	{
		printf("usage: %s input_filename offset_distance output_filename\n", argv[0]);
		return -1;
	}
	
	int i, j, k, l;					// general loop indices I always use
	
	std::string input_filename(argv[1]);		// name of mesh input file
	g_offset_distance = atof( argv[2] );
	std::string output_filename(argv[3]);		// name of mesh input file
	
	
	
	// READ OBJ FILE
	std::vector<SlVector3> obj_vertices;
	std::vector<SlTri> obj_triangles;
	
	bool success = readObjFile((char *)input_filename.c_str(), obj_vertices, obj_triangles);
	if (!success)
	{
		fprintf(stderr, "(opening input mesh file) error: unable to read OBJ file.\n");
		return false;
	}
	
	
	
	// COMPUTE TRIANGLE MESH BOUNDING BOX
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
	
	SlVector3 diagonal = cornerMax - cornerMin;
	center = (cornerMin + cornerMax) / 2.0;
	double max_dim = std::max( diagonal[0], std::max( diagonal[1], diagonal[2] ) );
	cornerMin = (center - SlVector3( max_dim, max_dim, max_dim ) / 2.0) * 1.25;
	cornerMax = (center + SlVector3( max_dim, max_dim, max_dim ) / 2.0) * 1.25;
	
	
	
	// CREATE LEVEL SET SURFACE FOR THE TRIANGLE MESH
	
	// NOTE: the corners must be offset by a slight factor to make the distance field not fuck up
	if (!createLevelSetSurface(obj_vertices, obj_triangles, g_s, cornerMin, cornerMax, 7))
	{
		fprintf(stderr, "(creating level set from input mesh) error: unable to create level set.\n");
		return -1;
	}
	
	SlcSurface t;
	t.tree = new DtTree();
	buildTree( t.tree, cornerMin, cornerMax, 7, g_evalPhi );
	//t.computeNormals();
	//t.computePsuedoNormals();
	//t.redistance();
	
	/*
	// MOVE VERTICES ALONG NORMALS
	for (i = 0; i < s.meshPts.size(); i++)
	{
		g_s.meshPts[i] += s.normals[i] * g_offset_distance;
	}
	*/
	
	
	t.contourTree( g_evalPhi );
	
	
	// OUTPUT UPDATED OBJ FILE
	std::ofstream out_stream;
	out_stream.open((char *)output_filename.c_str());
	for (i = 0; i < t.meshPts.size(); i++)
	{
		out_stream << "v " << t.meshPts[i][0] << " " << t.meshPts[i][1] << " " << t.meshPts[i][2] << std::endl;
	}
	
	for (i = 0; i < t.triangles.size(); i++)
	{
		out_stream << "f " << t.triangles[i][0] + 1 << " " << t.triangles[i][1] + 1 << " " << t.triangles[i][2] + 1 << std::endl;
	}
	
	out_stream.close();
	
}

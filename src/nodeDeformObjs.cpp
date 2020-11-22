/*
WHY I WROTE THIS FILE:

I'm embedding surface meshes in deformed volume meshes for animation.

by Alex Stuart 'Linearity'
*/

#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "IndexTet.h"
#include "TetMesh.h"
#include "slMatrix.H"
#include "slVector.H"
#include "slUtil.H"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("usage: %s rest_node_filename deformed_node_list_filename in_obj_filename\n", argv[0]);
		exit(0);
	}
	
	char *rest_node_filename = argv[1];
	char *deformed_node_list_filename = argv[2];
	char *in_obj_filename = argv[3];
	
	char *rest_ele_filename = new char[strlen(rest_node_filename) + 1 - 1];
	memcpy(rest_ele_filename, rest_node_filename, strlen(rest_node_filename) - 4);
	memcpy(rest_ele_filename + strlen(rest_node_filename) - 4, "ele", 4);
	
	char *deformed_node_filename = new char[80];
	char *deformed_ele_filename = new char[80];
	char *out_obj_filename = new char[85];
	
	std::ifstream in_stream;
	std::ofstream out_stream;
	
	in_stream.open(rest_ele_filename);
	if (in_stream.fail())
	{
		fprintf(stderr, "error: no .ele file found matching %s (%s)\n", rest_node_filename, rest_ele_filename);
		exit(-1);
	}
	in_stream.close();

	
	
	
	
	int i, j, k;
	SlVector3 curr_vertex;
	SlVector3 *rest_tet_vertices, *deformed_tet_vertices, *out_obj_vertices;
	int tet_vertex_count = 0, obj_vertex_count = 0;
	IndexTet curr_tet;
	IndexTet *rest_tets, *deformed_tets;
	std::vector<SlVector3> obj_vertices;
	std::vector<SlTri> obj_triangles;
	int tet_count = 0;
	int deformed_node_file_count;
	int dummy;
	
	// read rest node file
	in_stream.open(rest_node_filename);
	in_stream >> tet_vertex_count;
	rest_tet_vertices = new SlVector3[tet_vertex_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < tet_vertex_count))
	{
		in_stream >> dummy >> curr_vertex[0] >> curr_vertex[1] >> curr_vertex[2];
		rest_tet_vertices[i] = curr_vertex;
		i++;
	}
	in_stream.close();
	
	// read rest node file
	in_stream.open(rest_ele_filename);
	in_stream >> tet_count;
	rest_tets = new IndexTet[tet_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < tet_count))
	{
		in_stream >> dummy >> curr_tet[0] >> curr_tet[1] >> curr_tet[2] >> curr_tet[3];
		curr_tet[0]--;
		curr_tet[1]--;
		curr_tet[2]--;
		curr_tet[3]--;
		rest_tets[i] = curr_tet;
		i++;
	}
	in_stream.close();
	
	// read deformed node list file
	in_stream.open(deformed_node_list_filename);
	in_stream >> deformed_node_file_count;
	char **deformed_node_filenames = new char *[deformed_node_file_count];
	for (i = 0; i < deformed_node_file_count; i++)
	{
		deformed_node_filenames[i] = new char[80];
		in_stream >> deformed_node_filenames[i];
	}
	in_stream.close();
	
	// read in obj file
	bool success = readObjFile(in_obj_filename, obj_vertices, obj_triangles);
	
	out_obj_vertices = new SlVector3[obj_vertices.size()];
	for ( i = 0; i < obj_vertices.size(); i++ )
	{
		out_obj_vertices[i] = obj_vertices[i];
	}
	
	
	
	
	TetMesh rest_tetmesh(rest_tet_vertices, tet_vertex_count, rest_tets, tet_count);	
	TetMesh *deformed_tetmesh;
	
	SlMatrix3x3 *beta_rest = new SlMatrix3x3[tet_count];
	
	for ( k = 0; k < tet_count; k++ )
	{
		curr_tet = *rest_tetmesh.getTetPointer(k);
		beta_rest[k] = inverse( SlMatrix3x3( *rest_tetmesh.getVertexPointer( curr_tet[0] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[1] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[2] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), SlMatrix3x3::col ) );
	}
	
	int embedding_tets[obj_vertices.size()];
	memset( embedding_tets, 0, obj_vertices.size() * sizeof(int) );
	
	for ( j = 0; j < obj_vertices.size(); j++ )
	{
		for ( k = 0; k < tet_count; k++ )
		{
			if ( rest_tetmesh.pointInTet( obj_vertices[j], rest_tetmesh.getTetPointer(k) ) )
			{
				embedding_tets[j] = k;
				break;
			}
		}
		assert( k < tet_count );
	}
	
	
	// replace rest vertex world-space coordinates with barycentric coordinates
	for ( i = 0; i < obj_vertices.size(); i++ )
	{
		assert( rest_tetmesh.pointInTet( obj_vertices[i], rest_tetmesh.getTetPointer(embedding_tets[i]) ) );
		curr_tet = *rest_tetmesh.getTetPointer(embedding_tets[i]);
		
		obj_vertices[i] = beta_rest[embedding_tets[i]] * (obj_vertices[i] - *rest_tetmesh.getVertexPointer( curr_tet[3] ));
	}
	
	SlMatrix3x3 B_deformed;
	
	for ( i = 0; i < deformed_node_file_count; i++ )
	{	
		in_stream.open(deformed_node_filenames[i] );
		in_stream >> tet_vertex_count;
		deformed_tet_vertices = new SlVector3[tet_vertex_count];
		in_stream.ignore(256, '\n');
		j = 0;
		while (!in_stream.eof() && (j < tet_vertex_count))
		{
			in_stream >> dummy >> curr_vertex[0] >> curr_vertex[1] >> curr_vertex[2];
			deformed_tet_vertices[j] = curr_vertex;
			j++;
		}
		in_stream.close();
		
		/*
		memcpy(deformed_ele_filename, deformed_node_filenames[i], strlen(deformed_node_filenames[i]) - 4);
		memcpy(deformed_ele_filename + strlen(deformed_node_filenames[i]) - 4, "ele", 4);
		
		in_stream.open(deformed_ele_filename);
		if (in_stream.fail())
		{
			fprintf(stderr, "error: no .ele file found matching %s (%s)\n", deformed_node_filename, deformed_ele_filename);
			continue;
		}
		in_stream >> tet_count;
		deformed_tets = new IndexTet[tet_count];
		in_stream.ignore(256, '\n');
		j = 0;
		while (!in_stream.eof() && (j < tet_count))
		{
			in_stream >> dummy >> curr_tet[0] >> curr_tet[1] >> curr_tet[2] >> curr_tet[3];
			curr_tet[0]--;
			curr_tet[1]--;
			curr_tet[2]--;
			curr_tet[3]--;
			deformed_tets[j] = curr_tet;
			j++;
		}
		in_stream.close();
		*/
		deformed_tets = rest_tets;
		
		deformed_tetmesh = new TetMesh(deformed_tet_vertices, tet_vertex_count, deformed_tets, tet_count);
		
		for ( j = 0; j < obj_vertices.size(); j++ )
		{
			curr_tet = *deformed_tetmesh->getTetPointer(embedding_tets[j]);
			
			B_deformed = SlMatrix3x3( *deformed_tetmesh->getVertexPointer( curr_tet[0] ) - *deformed_tetmesh->getVertexPointer( curr_tet[3] ), *deformed_tetmesh->getVertexPointer( curr_tet[1] ) - *deformed_tetmesh->getVertexPointer( curr_tet[3] ), *deformed_tetmesh->getVertexPointer( curr_tet[2] ) - *deformed_tetmesh->getVertexPointer( curr_tet[3] ), SlMatrix3x3::col );
		
			curr_tet = *rest_tetmesh.getTetPointer(embedding_tets[j]);
			
			out_obj_vertices[j] = *deformed_tetmesh->getVertexPointer( curr_tet[3] ) + B_deformed * obj_vertices[j];
		}
	
		// write out OBJ file
		sprintf( out_obj_filename, "%s.obj", deformed_node_filenames[i] );
		out_stream.open(out_obj_filename);
		for (j = 0; j < obj_vertices.size(); j++)
		{
			out_stream << "v " << out_obj_vertices[j][0] << " " << out_obj_vertices[j][1] << " " << out_obj_vertices[j][2] << std::endl;
		}
		
		for (j = 0; j < obj_triangles.size(); j++)
		{
			out_stream << "f " << obj_triangles[j][0] + 1 << " " << obj_triangles[j][1] + 1 << " " << obj_triangles[j][2] + 1 << std::endl;
		}
		out_stream.close();
		
		delete deformed_tetmesh;
	}
}

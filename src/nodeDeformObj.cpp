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
	if (argc != 5)
	{
		printf("usage: %s rest_node_filename deformed_node_filename in_obj_filename out_obj_filename\n", argv[0]);
		exit(0);
	}
	
	char *rest_node_filename = argv[1];
	char *deformed_node_filename = argv[2];
	char *in_obj_filename = argv[3];
	char *out_obj_filename = argv[4];
	
	char *rest_ele_filename = new char[strlen(rest_node_filename) - 1];
	memcpy(rest_ele_filename, rest_node_filename, strlen(rest_node_filename) - 4);
	memcpy(rest_ele_filename + strlen(rest_node_filename) - 4, "ele", 3);
	
	char *deformed_ele_filename = new char[strlen(deformed_node_filename) - 1];
	memcpy(deformed_ele_filename, deformed_node_filename, strlen(deformed_node_filename) - 4);
	memcpy(deformed_ele_filename + strlen(deformed_node_filename) - 4, "ele", 3);
	
	std::ifstream in_stream;
	std::ofstream out_stream;
	
	in_stream.open(rest_ele_filename);
	if (in_stream.fail())
	{
		fprintf(stderr, "error: no .ele file found matching %s (%s)\n", rest_node_filename, rest_ele_filename);
		exit(-1);
	}
	in_stream.close();
	
	in_stream.open(deformed_ele_filename);
	if (in_stream.fail())
	{
		fprintf(stderr, "error: no .ele file found matching %s (%s)\n", deformed_node_filename, deformed_ele_filename);
		exit(-1);
	}
	in_stream.close();
	
	
	
	// read files
	int i, j;
	SlVector3 curr_vertex;
	SlVector3 *rest_tet_vertices, *deformed_tet_vertices, *out_obj_vertices;
	int tet_vertex_count = 0, obj_vertex_count = 0;
	IndexTet curr_tet;
	IndexTet *rest_tets, *deformed_tets;
	int tet_count = 0;
	int dummy;
	
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
	
	TetMesh rest_tetmesh(rest_tet_vertices, tet_vertex_count, rest_tets, tet_count);
	
	
	
	in_stream.open(deformed_node_filename );
	in_stream >> tet_vertex_count;
	deformed_tet_vertices = new SlVector3[tet_vertex_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < tet_vertex_count))
	{
		in_stream >> dummy >> curr_vertex[0] >> curr_vertex[1] >> curr_vertex[2];
		deformed_tet_vertices[i] = curr_vertex;
		i++;
	}
	in_stream.close();
	
	in_stream.open(deformed_ele_filename);
	in_stream >> tet_count;
	deformed_tets = new IndexTet[tet_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < tet_count))
	{
		in_stream >> dummy >> curr_tet[0] >> curr_tet[1] >> curr_tet[2] >> curr_tet[3];
		curr_tet[0]--;
		curr_tet[1]--;
		curr_tet[2]--;
		curr_tet[3]--;
		deformed_tets[i] = curr_tet;
		i++;
	}
	in_stream.close();
	
	TetMesh deformed_tetmesh(deformed_tet_vertices, tet_vertex_count, deformed_tets, tet_count);
	
	
	
	std::vector<SlVector3> obj_vertices;
	std::vector<SlTri> obj_triangles;
	bool success = readObjFile(in_obj_filename, obj_vertices, obj_triangles);
	
	out_obj_vertices = new SlVector3[obj_vertices.size()];
	for ( j = 0; j < obj_vertices.size(); j++ )
	{
		out_obj_vertices[j] = obj_vertices[j];
	}
	
	SlMatrix3x3 beta_rest, B_deformed;
	beta_rest.setIdentity();
	B_deformed.setIdentity();
	
	/*
	for ( i = 0; i < tet_count; i++ )
	{
		curr_tet = *deformed_tetmesh.getTetPointer(i);
		B_deformed = SlMatrix3x3( *deformed_tetmesh.getVertexPointer( curr_tet[0] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), *deformed_tetmesh.getVertexPointer( curr_tet[1] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), *deformed_tetmesh.getVertexPointer( curr_tet[2] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), SlMatrix3x3::col );

		curr_tet = *rest_tetmesh.getTetPointer(i);
		beta_rest = inverse( SlMatrix3x3( *rest_tetmesh.getVertexPointer( curr_tet[0] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[1] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[2] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), SlMatrix3x3::col ) );
				
		for ( j = 0; j < obj_vertices.size(); j++ )
		{
			if ( rest_tetmesh.pointInTet( obj_vertices[j], rest_tetmesh.getTetPointer(i) ) )
			{
				out_obj_vertices[j] = *deformed_tetmesh.getVertexPointer( curr_tet[3] ) + B_deformed * beta_rest * (obj_vertices[j] - *rest_tetmesh.getVertexPointer( curr_tet[3] ));
			}
		}
	}
	*/
	
	for ( j = 0; j < obj_vertices.size(); j++ )
	{
		for ( i = 0; i < tet_count; i++ )
		{
			curr_tet = *deformed_tetmesh.getTetPointer(i);
			B_deformed = SlMatrix3x3( *deformed_tetmesh.getVertexPointer( curr_tet[0] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), *deformed_tetmesh.getVertexPointer( curr_tet[1] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), *deformed_tetmesh.getVertexPointer( curr_tet[2] ) - *deformed_tetmesh.getVertexPointer( curr_tet[3] ), SlMatrix3x3::col );
	
			curr_tet = *rest_tetmesh.getTetPointer(i);
			beta_rest = inverse( SlMatrix3x3( *rest_tetmesh.getVertexPointer( curr_tet[0] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[1] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), *rest_tetmesh.getVertexPointer( curr_tet[2] ) - *rest_tetmesh.getVertexPointer( curr_tet[3] ), SlMatrix3x3::col ) );
			if ( rest_tetmesh.pointInTet( obj_vertices[j], rest_tetmesh.getTetPointer(i) ) )
			{
				out_obj_vertices[j] = *deformed_tetmesh.getVertexPointer( curr_tet[3] ) + B_deformed * beta_rest * (obj_vertices[j] - *rest_tetmesh.getVertexPointer( curr_tet[3] ));
				break;
			}
		}
		if ( i < tet_count )
		{
			//out_obj_vertices[j][1] += 1000.0;
		}
	}
	
	/*
	SlTri faces[4 * rest_tetmesh.tet_count];
	int face_count;
	// write OBJ file
	out_stream.open(out_obj_filename);
	
	for (i = 0; i < rest_tetmesh.vertex_count; i++)
	{
		out_stream << "v " << rest_tetmesh.vertex_array[i][0] << " " << rest_tetmesh.vertex_array[i][1] << " " << rest_tetmesh.vertex_array[i][2] << std::endl;
	}
	
	rest_tetmesh.extractFaces(faces, &face_count);	
	
	for (i = 0; i < face_count; i++)
	{
		out_stream << "f " << faces[i][0] + 1 << " " << faces[i][1] + 1 << " " << faces[i][2] + 1 << std::endl;
	}
	*/

	// write out OBJ file
	out_stream.open(out_obj_filename);
	for (i = 0; i < obj_vertices.size(); i++)
	{
		out_stream << "v " << out_obj_vertices[i][0] << " " << out_obj_vertices[i][1] << " " << out_obj_vertices[i][2] << std::endl;
	}
	
	for (i = 0; i < obj_triangles.size(); i++)
	{
		out_stream << "f " << obj_triangles[i][0] + 1 << " " << obj_triangles[i][1] + 1 << " " << obj_triangles[i][2] + 1 << std::endl;
	}
	out_stream.close();
}

/*
WHY I WROTE THIS FILE:

I needed the TetMesh surface extraction code node/ele to obj file conversion.

by Alex Stuart 'Linearity'
*/

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "IndexTet.h"
#include "TetMesh.h"
#include "slVector.H"
#include "slUtil.H"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: %s node_filename\n", argv[0]);
		exit(0);
	}
	
	char *node_filename = argv[1];
	char *obj_filename = argv[2];
	
	char *ele_filename = new char[strlen(node_filename) - 1];
	memcpy(ele_filename, node_filename, strlen(node_filename) - 4);
	memcpy(ele_filename + strlen(node_filename) - 4, "ele", 3);
	
	std::ifstream in_stream;
	std::ofstream out_stream;
	
	in_stream.open(ele_filename);
	if (in_stream.fail())
	{
		fprintf(stderr, "error: no .ele file found matching %s (%s)\n", node_filename, ele_filename);
		exit(-1);
	}
	in_stream.close();
	
	// read files
	int i;
	SlVector3 curr_vertex;
	SlVector3 *vertices;
	int vertex_count = 0;
	IndexTet curr_tet;
	IndexTet *tets;
	int tet_count = 0;
	int dummy;
	
	in_stream.open(node_filename);
	in_stream >> vertex_count;
	vertices = new SlVector3[vertex_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < vertex_count))
	{
		in_stream >> dummy >> curr_vertex[0] >> curr_vertex[1] >> curr_vertex[2];
		vertices[i] = curr_vertex;
		i++;
	}
	in_stream.close();
	
	in_stream.open(ele_filename);
	in_stream >> tet_count;
	tets = new IndexTet[tet_count];
	in_stream.ignore(256, '\n');
	i = 0;
	while (!in_stream.eof() && (i < tet_count))
	{
		in_stream >> dummy >> curr_tet[0] >> curr_tet[1] >> curr_tet[2] >> curr_tet[3];
		curr_tet[0]--;
		curr_tet[1]--;
		curr_tet[2]--;
		curr_tet[3]--;
		tets[i] = curr_tet;
		i++;
	}
	in_stream.close();
	
	
	
	// create tetmesh
	TetMesh tetmesh(vertices, vertex_count, tets, tet_count);
	
	tetmesh.reorientTets();
	
	
	// write out ELE file
	out_stream.open(ele_filename);
	out_stream << tetmesh.tet_count << " " << 4 << " " << 0 << " " << std::endl;
	for (i = 0; i < tetmesh.tet_count; i++)
	{
		out_stream << i + 1 << " " << (*tetmesh.getTetPointer(i))[0] + 1 << " " << (*tetmesh.getTetPointer(i))[1] + 1 << " " << (*tetmesh.getTetPointer(i))[2] + 1 << " " << (*tetmesh.getTetPointer(i))[3] + 1 << std::endl;
	}
	out_stream.close();
}

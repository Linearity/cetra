/*
WHY I WROTE THIS FILE:

I am generating an implicit surface from a triangle mesh.

by Alex Stuart 'Linearity'
*/

#include <assert.h>
#include <stdio.h>
#include <vector>

#include "slVector.H"
#include "slUtil.H"
#include "SurfaceTracking/slcSurface.H"
#include "SurfaceTracking/distTree.H"

// from obj2mesh.cpp
bool createLevelSetSurface(std::vector<SlVector3>& vertices, std::vector<SlTri>& triangles, SlcSurface &s, SlVector3 boundingBoxCornerMin, SlVector3 boundingBoxCornerMax, int treeLevel)
{
	std::vector<SlVector3> vertexNormals, faceNormals;
	
	/*
	// ensure proper orientation of triangles
	int temp;
	for (std::vector<SlTri>::iterator triIter = triangles.begin(); triIter != triangles.end(); triIter++)
	{		
		if (	mag(	cross(	(vertices[(*triIter)[1]] - vertices[(*triIter)[0]]),
					(vertices[(*triIter)[2]] - vertices[(*triIter)[0]])	)	) < 0)
		{
			temp = (*triIter)[1];
			(*triIter)[1] = (*triIter)[2];
			(*triIter)[2] = temp;
		}
	}
	*/
	
	s.tree = new DtTree();
	computeNormals(vertices, triangles, vertexNormals, faceNormals);
	buildTree(s.tree, boundingBoxCornerMin, boundingBoxCornerMax, treeLevel, &vertices, &triangles, &faceNormals);

	s.meshPts = vertices;
	s.triangles = triangles;

	s.computeNormals();
	s.computePsuedoNormals();
	s.redistance();
	
	return true;
}

/*
WHY I'M WRITING THIS FILE:

I'm implementing the element representation in TetElement.h

by Alex Stuart 'Linearity'
*/

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "slMatrix.H"
#include "slVector.H"
#include "TetElement.h"

SlVector3 projection(const SlVector3& a, const SlVector3& b)
{
	return (a * b) / (b * b) * b;
}

TetElement::TetElement() : volume(0.0)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		nodes[i] = NULL;
		areaWeightedNormals[i] = SlVector3(0.0, 0.0, 0.0);
	}
}

TetElement::TetElement(Node& v1, Node& v2, Node& v3, Node& v4) : volume(fabs(dot(v1.position - v4.position, cross(v2.position - v4.position, v3.position - v4.position))) / 6.0)
{
	nodes[0] = &v1;
	nodes[1] = &v2;
	nodes[2] = &v3;
	nodes[3] = &v4;
	
	computeNormals();
	computeBeta();
}

void TetElement::computeNormals()
{
	SlVector3 a, b;
	
	int i;
	for (i = 0; i < 4; i++)
	{
		// the vectors defining the current face of the tetrahedron
		a = nodes[(i + 1) % 4]->position - nodes[i]->position;
		b = nodes[(i + 2) % 4]->position - nodes[i]->position;
		
		// normalized cross product scaled by area of triangle given by vectors a and b
		// NOTE: depending on how the tet is oriented, the normals may be all the wrong direction
		areaWeightedNormals[i] = pow(-1.0, i + 1) * (cross(a, b) / mag(cross(a, b))) * (0.5 * mag(a) * mag(b - projection(b, a)));
		
		#if DEBUG
			std::cerr << "(computing normals) normal vector " << i << ": [" << areaWeightedNormals[i].x() << ", " << areaWeightedNormals[i].y() << ", " << areaWeightedNormals[i].z() << "]." << std::endl;
			std::cerr << "(computing normals) normal vector " << i << " computed from edges [" << a.x() << ", " << a.y() << ", " << a.z() << "] (edge " << i << (i + 1) % 4 << ") and [" << b.x() << ", " << b.y() << ", " << b.z() << "] (edge " << i << (i + 2) % 4 << ")." << std::endl;
		#endif
	}
}

void TetElement::computeVolume()
{
	volume = fabs(dot(nodes[0]->position - nodes[3]->position, cross(nodes[1]->position - nodes[3]->position, nodes[2]->position - nodes[3]->position))) / 6.0;
}

void TetElement::computeBeta()
{
	SlMatrix3x3 betaInverse = SlMatrix3x3(nodes[0]->position - nodes[3]->position, nodes[1]->position - nodes[3]->position, nodes[2]->position - nodes[3]->position, SlMatrix3x3::col);
	beta = inverse(betaInverse);
}

SlVector3 TetElement::center()
{
	return 0.25 * (nodes[0]->position + nodes[1]->position + nodes[2]->position + nodes[3]->position);
}

TetElement& TetElement::operator=(const TetElement& rhs)
{
	if (&rhs != this)
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			nodes[i] = rhs.nodes[i];
			areaWeightedNormals[i] = rhs.areaWeightedNormals[i];
		}
		volume = rhs.volume;
	}	
	return *this;
}

#ifndef __TET_ELEMENT__
#define __TET_ELEMENT__

/*
WHY I'M WRITING THIS FILE:

I need a way to group Node objects together into tetrahedral elements.

I'm grouping together element-specific data and functions for calculating them

by Alex Stuart 'Linearity'
*/

#include "Node.h"
#include "slMatrix.H"
#include "slVector.H"

class TetElement
{
	public:
		TetElement();
		TetElement(Node& v1, Node& v2, Node& v3, Node& v4);
		
		void computeNormals();
		void computeVolume();
		void computeBeta();
		
		SlVector3 center();
		
		TetElement& operator=(const TetElement& rhs);
		
		Node *nodes[4];
		SlVector3 areaWeightedNormals[4];
		float volume;
		SlMatrix3x3 beta;
};

#endif

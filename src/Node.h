#ifndef __NODE__
#define __NODE__

/*
WHY I'M WRITING THIS FILE:

I want to define a class representing nodes in a mesh for finite-element strain simulation.

by Alex Stuart 'Linearity'
*/

#include "slVector.H"

class Node
{
	public:
		Node();
		Node(SlVector3 pos);
		Node(SlVector3 pos, SlVector3 vel, float m, SlVector3 F);
		
		SlVector3 position;
		SlVector3 velocity;
		float mass;
		SlVector3 force;
};

#endif

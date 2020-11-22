/*
WHY I'M WRITING THIS FILE:

I'm implementing the node class in Node.h.

by Alex Stuart 'Linearity'
*/

#include "Node.h"

Node::Node() : position(SlVector3(0.0, 0.0, 0.0)), velocity(SlVector3(0.0, 0.0, 0.0)), mass(0.0), force(SlVector3(0.0, 0.0, 0.0))
{
	// everything handled by initializer list at time of writing
}

Node::Node(SlVector3 pos) : position(pos), velocity(SlVector3(0.0, 0.0, 0.0)), mass(0.0), force(SlVector3(0.0, 0.0, 0.0))
{
	// everything handled by initializer list at time of writing
}

Node::Node(SlVector3 pos, SlVector3 vel, float m, SlVector3 F) : position(pos), velocity(vel), mass(m), force(F)
{
	// everything handled by initializer list at time of writing
}

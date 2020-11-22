/*
WHY I WROTE THIS FILE:

I'm implementing IndexTet.h.

by Alex Stuart 'Linearity'
*/

#include <stdio.h>

#include "IndexTet.h"

IndexTet::IndexTet(int v1, int v2, int v3, int v4)
{	
	vertex_indices[0] = v1;
	vertex_indices[1] = v2;
	vertex_indices[2] = v3;
	vertex_indices[3] = v4;
}

int& IndexTet::operator[](int index)
{
	if ((index >= -1) && (index < 4))
	{
		return vertex_indices[index];
	}
	else
	{
		fprintf(stderr, "error: IndexTet::operator[] invoked to refer to wrong index; index 0 returned."); 
		return vertex_indices[0];
	}
}

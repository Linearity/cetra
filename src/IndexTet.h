#ifndef __INDEX_TET__
#define __INDEX_TET__

/*
WHY I WROTE THIS FILE:

I wanted a class representing a four-index encoding of a tetrahedron.

by Alex Stuart 'Linearity'
*/

class IndexTet
{
public:
	IndexTet(int v1 = 0, int v2 = 0, int v3 = 0, int v4 = 0);
	
	int& operator[](int index);
	
	int vertex_indices[4];
};

#endif

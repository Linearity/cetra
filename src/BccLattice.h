#ifndef __BCC_LATTICE__
#define __BCC_LATTICE__

/*
WHY I WROTE THIS FILE:

I'm putting the enum from BccLattice.cpp and the function declarations from main.cpp in one file.

by Alex Stuart 'Linearity', at 4:55 PM on November 18, 2011
*/

#include "IndexTet.h"
#include "slVector.H"

enum bccNodeGroups
{
	OUTER_X0,
	OUTER_Y0,
	OUTER_Z0,
	GRID,
	INNER,
	GRID_ZMAX,
	OUTER_ZMAX,
	GRID_YMAX,
	OUTER_YMAX,
	GRID_YMAX_ZMAX,
	GRID_XMAX,
	OUTER_XMAX,
	GRID_XMAX_ZMAX,
	GRID_XMAX_YMAX,
	GRID_XMAX_YMAX_ZMAX
};

int getBccIndex(int group, unsigned int x, unsigned int y, unsigned int z, unsigned int nx, unsigned int ny, unsigned int nz);
int getBccGridIndex(unsigned int x, unsigned int y, unsigned int z, unsigned int nx, unsigned int ny, unsigned int nz);
int getBccCenterIndex(int x, int y, int z, unsigned int nx, unsigned int ny, unsigned int nz);

SlVector3 *getBccVertex(int group, unsigned int x, unsigned int y, unsigned int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz);
SlVector3 *getBccGridVertex(unsigned int x, unsigned int y, unsigned int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz);
SlVector3 *getBccCenterVertex(int x, int y, int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz);

int countBccVertices(unsigned int nx, unsigned int ny, unsigned int nz);
int countBccTets(unsigned int nx, unsigned int ny, unsigned int nz);

void generateBccLattice(unsigned int nx, unsigned int ny, unsigned int nz, double h, SlVector3 origin, SlVector3 *vertex_memory, IndexTet *tet_memory);

#endif

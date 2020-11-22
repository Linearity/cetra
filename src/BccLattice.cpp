/*
WHY I WROTE THIS FILE:

I want to write a function that constructs and stores a representation of
a tetrahedral mesh based on a body-centered cubic (BCC) lattice.

by Alex Stuart 'Linearity', at 4:04 PM on November 16, 2011
*/

#include "BccLattice.h"
#include "IndexTet.h"
#include "slVector.H"

typedef uint = unsigned int

int getBccIndex(int group, unsigned int x, unsigned int y, unsigned int z, unsigned int nx, unsigned int ny, unsigned int nz)
{
	switch (group)
	{
		case OUTER_X0:
			if ((y < ny) && (z < nz))
			{
				return nz * y + z;
			}
			else
			{
				return -1;
			}
			break;
		case OUTER_Y0:
			if ((x < nx) && (z < nz))
			{
				return ny * nz + (nz + (2 * nz + 3) * ny + 2 * nz + 1) * x + z;
			}
			else
			{
				return -1;
			}
			break;
		case OUTER_Z0:
			if ((x < nx) && (y < ny))
			{
				return ny * nz + nz + (nz + (2 * nz + 3) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * y;
			}
			else
			{
				return -1;
			}
			break;
		case GRID:
			if ((x < nx) && (y < ny) && (z < nz))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * y + 1 + 2 * z;
			}
			else
			{
				return -1;
			}
			break;
		case INNER:
			if ((x < nx) && (y < ny) && (z < nz))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * y + 1 + 2 * z + 1;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_ZMAX:
			if ((x < nx) && (y < ny))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * y + 1 + 2 * nz;
			}
			else
			{
				return -1;
			}
			break;
		case OUTER_ZMAX:
			if ((x < nx) && (y < ny))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * y + 1 + 2 * nz + 1;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_YMAX:
			if ((x < nx) && (z < nz))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * ny + 2 * z;
			}
			else
			{
				return -1;
			}
			break;
		case OUTER_YMAX:
			if ((x < nx) && (z < nz))
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * ny + 2 * z + 1;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_YMAX_ZMAX:
			if (x < nx)
			{
				return ny * nz + nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * x + (1 + 2 * nz + 2) * ny + 2 * nz;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_XMAX:
			if ((y < ny) && (z < nz))
			{
				return ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * y + 2 * z;
			}
			else
			{
				return -1;
			}
			break;
		case OUTER_XMAX:
			if ((y < ny) && (z < nz))
			{
				return ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * y + 2 * z + 1;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_XMAX_ZMAX:
			if (y < ny)
			{
				return ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * y + 2 * nz;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_XMAX_YMAX:
			if (z < nz)
			{
				return ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * ny + z;
			}
			else
			{
				return -1;
			}
			break;
		case GRID_XMAX_YMAX_ZMAX:
			return ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * ny + nz;
			break;
		default:
			return -1;
	}
}

int getBccGridIndex(unsigned int x, unsigned int y, unsigned int z, unsigned int nx, unsigned int ny, unsigned int nz)
{
	if (x < nx)
	{
		if (y < ny)
		{
			if (z < nz)
			{
				return getBccIndex(GRID, x, y, z, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccIndex(GRID_ZMAX, x, y, 0, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else if (y == ny)
		{
			if (z < nz)
			{
				return getBccIndex(GRID_YMAX, x, 0, z, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccIndex(GRID_YMAX_ZMAX, x, 0, 0, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if (x == nx)
	{
		if (y < ny)
		{
			if (z < nz)
			{
				return getBccIndex(GRID_XMAX, 0, y, z, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccIndex(GRID_XMAX_ZMAX, 0, y, 0, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else if (y == ny)
		{
			if (z < nz)
			{
				return getBccIndex(GRID_XMAX_YMAX, 0, 0, z, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccIndex(GRID_XMAX_YMAX_ZMAX, 0, 0, 0, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int getBccCenterIndex(int x, int y, int z, unsigned int nx, unsigned int ny, unsigned int nz)
{
	if (x == -1)
	{
		if ((y >= 0) && (y < ny))
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccIndex(OUTER_X0, 0, y, z, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if ((x >= 0) && (x < nx))
	{
		if (y == -1)
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccIndex(OUTER_Y0, x, 0, z, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else if ((y >= 0) && (y < ny))
		{
			if (z == -1)
			{
				return getBccIndex(OUTER_Z0, x, y, 0, nx, ny, nz);
			}
			else if ((z >= 0) && (z < nz))
			{
				return getBccIndex(INNER, x, y, z, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccIndex(OUTER_ZMAX, x, y, 0, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else if (y == ny)
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccIndex(OUTER_YMAX, x, 0, z, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if (x == nx)
	{
		if ((y >= 0) && (y < ny))
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccIndex(OUTER_XMAX, 0, y, z, nx, ny, nz);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

SlVector3 *getBccVertex(int group, unsigned int x, unsigned int y, unsigned int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz)
{
	int index = getBccIndex(group, x, y, z, nx, ny, nz);
	if (index == -1)
	{
		return NULL;
	}
	else
	{
		return &vertices[index];
	}
}

SlVector3 *getBccGridVertex(unsigned int x, unsigned int y, unsigned int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz)
{
	if (x < nx)
	{
		if (y < ny)
		{
			if (z < nz)
			{
				return getBccVertex(GRID, x, y, z, vertices, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccVertex(GRID_ZMAX, x, y, 0, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else if (y == ny)
		{
			if (z < nz)
			{
				return getBccVertex(GRID_YMAX, x, 0, z, vertices, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccVertex(GRID_YMAX_ZMAX, x, 0, 0, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else if (x == nx)
	{
		if (y < ny)
		{
			if (z < nz)
			{
				return getBccVertex(GRID_XMAX, 0, y, z, vertices, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccVertex(GRID_XMAX_ZMAX, 0, y, 0, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else if (y == ny)
		{
			if (z < nz)
			{
				return getBccVertex(GRID_XMAX_YMAX, 0, 0, z, vertices, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccVertex(GRID_XMAX_YMAX_ZMAX, 0, 0, 0, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

SlVector3 *getBccCenterVertex(int x, int y, int z, SlVector3 *vertices, unsigned int nx, unsigned int ny, unsigned int nz)
{
	if (x == -1)
	{
		if ((y >= 0) && (y < ny))
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccVertex(OUTER_X0, 0, y, z, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else if ((x >= 0) && (x < nx))
	{
		if (y == -1)
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccVertex(OUTER_Y0, x, 0, z, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else if ((y >= 0) && (y < ny))
		{
			if (z == -1)
			{
				return getBccVertex(OUTER_Z0, x, y, 0, vertices, nx, ny, nz);
			}
			else if ((z >= 0) && (z < nz))
			{
				return getBccVertex(INNER, x, y, z, vertices, nx, ny, nz);
			}
			else if (z == nz)
			{
				return getBccVertex(OUTER_ZMAX, x, y, 0, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else if (y == ny)
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccVertex(OUTER_YMAX, x, 0, z, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else if (x == nx)
	{
		if ((y >= 0) && (y < ny))
		{
			if ((z >= 0) && (z < nz))
			{
				return getBccVertex(OUTER_XMAX, 0, y, z, vertices, nx, ny, nz);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

int countBccVertices(unsigned int nx, unsigned int ny, unsigned int nz)
{
	return ny * nz + nx * nz + 3 * nx * ny + 2 * nx * ny * nz + 2 * nx * nz + nx + 2 * ny * nz + ny + nz + 1;
}

int countBccTets(unsigned int nx, unsigned int ny, unsigned int nz)
{
	return nx * ny * (nz + 1) * 4 + nx * nz * (ny + 1) * 4 + ny * nz * (nx + 1) * 4;
}

void generateBccLattice(unsigned int nx, unsigned int ny, unsigned int nz, double h, SlVector3 origin, SlVector3 *vertex_memory, IndexTet *tet_memory)
{
	int i, j, k;
	
	int vertex_index = 0;
	int tet_index = 0;
	
	/* CREATE VERTICES */
	for (j = 0; j < ny; j++)
	{
		for (k = 0; k < nz; k++)
		{
			// outer verticess for x = 0 side [vertices nz * j + k for all j, k such that 0 <= j < ny, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(0 - h / 2.0, j * h + h / 2.0, k * h + h / 2.0);
			vertex_index++;
		}
	}
	
	for (i = 0; i < nx; i++)
	{
		for (k = 0; k < nz; k++)
		{			
			// outer vertices for y = 0 side [vertices ny * nz + (nz + (2 * nz + 3) * ny + 2 * nz + 1) * i + k, for all i, k such that 0 <= i < nx, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(i * h + h / 2.0, 0 - h / 2.0, k * h + h / 2.0);
			vertex_index++;
		}
		
		for (j = 0; j < ny; j++)
		{
			// outer vertices for z = 0 side [vertices ny * nz + nz + (nz + (2 * nz + 3) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * j, for all i, j such that 0 <= i < nx, 0 <= j < ny]
			vertex_memory[vertex_index] = origin + SlVector3(i * h + h / 2.0, j * h + h / 2.0, -h / 2.0);
			vertex_index++;
			
			for (k = 0; k < nz; k++)
			{
				// grid vertices [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * j + k, for all i, j, k such that 0 <= i < nx, 0 <= j < ny, 0 <= k < nz]
				vertex_memory[vertex_index] = origin + SlVector3(i * h, j * h, k * h);
				vertex_index++;
				
				// inner vertices [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * j + k + 1, for all i, j, k such that 0 <= i < nx, 0 <= j < ny, 0 <= k < nz]
				vertex_memory[vertex_index] = origin + SlVector3(i * h + h / 2.0, j * h + h / 2.0, k * h + h / 2.0);
				vertex_index++;
			}
			
			// grid vertices for z = nz side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz) * j, for all i, j such that 0 <= i < nx, 0 <= j < ny]
			vertex_memory[vertex_index] = origin + SlVector3(i * h, j * h, nz * h);
			vertex_index++;
			
			// outer vertices for z = nz side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz) * j + 1, for all i, j such that 0 <= i < nx, 0 <= j < ny]
			vertex_memory[vertex_index] = origin + SlVector3(i * h + h / 2.0, j * h + h / 2.0, nz * h + h / 2.0);
			vertex_index++;
		}

		for (k = 0; k < nz; k++)
		{
			// grid vertices for y = ny side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * ny + 2 * k, for all i, k such that 0 <= i < nx, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(i * h, ny * h, k * h);
			vertex_index++;
			
			// outer vertices for y = ny side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * ny + 2 * k + 1, for all i, k such that 0 <= i < nx, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(i * h + h / 2.0, ny * h + h / 2.0, k * h + h / 2.0);
			vertex_index++;
		}
		// grid vertices for y = ny, z = nz edge [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * i + (1 + 2 * nz + 2) * ny + 2 * nz for all i such that 0 <= i < nx]
		vertex_memory[vertex_index] = origin + SlVector3(i * h, ny * h, nz * h);
		vertex_index++;
	}
	
	for (j = 0; j < ny; j++)
	{
		for (k = 0; k < nz; k++)
		{
			// grid vertices for x = nx side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * j + 2 * k, for all j, k such that 0 <= j < ny, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(nx * h, j * h, k * h);
			vertex_index++;
			
			// outer vertices for x = nx side [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * j + 2 * k + 1, for all j, k such that 0 <= j < ny, 0 <= k < nz]
			vertex_memory[vertex_index] = origin + SlVector3(nx * h + h / 2.0, j * h + h / 2.0, k * h + h / 2.0);
			vertex_index++;
		}
		// grid vertices for x = nx, z = nz edge [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * j + 2 * nz, for all j such that 0 <= j < ny]
		vertex_memory[vertex_index] = origin + SlVector3(nx * h, j * h, nz * h);
		vertex_index++;
	}

	for (k = 0; k < nz; k++)
	{
		// grid vertices for x = nx, y = ny edge [vertices ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * ny + k, for all k such that 0 <= k < nz]
		vertex_memory[vertex_index] = origin + SlVector3(nx * h, ny * h, k * h);
		vertex_index++;
	}
	
	// grid node for x = nx, y = ny, z = nz corner [node ny * nz + (nz + (1 + 2 * nz + 2) * ny + 2 * nz + 1) * nx + (2 * nz + 1) * ny + nz]
	vertex_memory[vertex_index] = origin + SlVector3(nx * h, ny * h, nz * h);

	/* CREATE TETRAHEDRA */
	int axis_a, axis_b;
	int corners[4];
	int t;
	
	// z axes
	for (i = 0; i < nx; i++)
	{
		for (j = 0; j < ny; j++)
		{
			// set axis of rotation
			axis_a = getBccCenterIndex(i, j, -1, nx, ny, nz);
			axis_b = getBccCenterIndex(i, j, 0, nx, ny, nz);
			
			// set corners for rotation
			corners[0] = getBccGridIndex(i, j, 0, nx, ny, nz);
			corners[1] = getBccGridIndex(i, j + 1, 0, nx, ny, nz);
			corners[2] = getBccGridIndex(i + 1, j + 1, 0, nx, ny, nz);
			corners[3] = getBccGridIndex(i + 1, j, 0, nx, ny, nz);
			
			// create four tets around the axis
			for (t = 0; t < 4; t++)
			{
				tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
				tet_index++;
			}
			
			for (k = 0; k < nz; k++)
			{
				// set axis of rotation
				axis_a = getBccCenterIndex(i, j, k, nx, ny, nz);
				axis_b = getBccCenterIndex(i, j, k + 1, nx, ny, nz);
				
				// set corners for rotation
				corners[0] = getBccGridIndex(i, j, k + 1, nx, ny, nz);
				corners[1] = getBccGridIndex(i, j + 1, k + 1, nx, ny, nz);
				corners[2] = getBccGridIndex(i + 1, j + 1, k + 1, nx, ny, nz);
				corners[3] = getBccGridIndex(i + 1, j, k + 1, nx, ny, nz);
				
				// create four tets around the axis
				for (t = 0; t < 4; t++)
				{
					tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
					tet_index++;
				}
			}
		}
	}
	
	// y axes
	for (i = 0; i < nx; i++)
	{
		for (k = 0; k < nz; k++)
		{
			// set axis of rotation
			axis_a = getBccCenterIndex(i, -1, k, nx, ny, nz);
			axis_b = getBccCenterIndex(i, 0, k, nx, ny, nz);
			
			// set corners for rotation
			corners[0] = getBccGridIndex(i, 0, k, nx, ny, nz);
			corners[1] = getBccGridIndex(i + 1, 0, k, nx, ny, nz);
			corners[2] = getBccGridIndex(i + 1, 0, k + 1, nx, ny, nz);
			corners[3] = getBccGridIndex(i, 0, k + 1, nx, ny, nz);
			
			// create four tets around the axis
			for (t = 0; t < 4; t++)
			{
				tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
				tet_index++;
			}
			
			for (j = 0; j < ny; j++)
			{
				// set axis of rotation
				axis_a = getBccCenterIndex(i, j, k, nx, ny, nz);
				axis_b = getBccCenterIndex(i, j + 1, k, nx, ny, nz);
				
				// set corners for rotation
				corners[0] = getBccGridIndex(i, j + 1, k, nx, ny, nz);
				corners[1] = getBccGridIndex(i + 1, j + 1, k, nx, ny, nz);
				corners[2] = getBccGridIndex(i + 1, j + 1, k + 1, nx, ny, nz);
				corners[3] = getBccGridIndex(i, j + 1, k + 1, nx, ny, nz);
				
				// create four tets around the axis
				for (t = 0; t < 4; t++)
				{
					tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
					tet_index++;
				}
			}
		}
	}
	
	// x axes
	for (j = 0; j < ny; j++)
	{
		for (k = 0; k < nz; k++)
		{
			// set axis of rotation
			axis_a = getBccCenterIndex(-1, j, k, nx, ny, nz);
			axis_b = getBccCenterIndex(0, j, k, nx, ny, nz);
			
			// set corners for rotation
			corners[0] = getBccGridIndex(0, j, k, nx, ny, nz);
			corners[1] = getBccGridIndex(0, j, k + 1, nx, ny, nz);
			corners[2] = getBccGridIndex(0, j + 1, k + 1, nx, ny, nz);
			corners[3] = getBccGridIndex(0, j + 1, k, nx, ny, nz);
			
			// create four tets around the axis
			for (t = 0; t < 4; t++)
			{
				tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
				tet_index++;
			}
			
			for (i = 0; i < nx; i++)
			{
				// set axis of rotation
				axis_a = getBccCenterIndex(i, j, k, nx, ny, nz);
				axis_b = getBccCenterIndex(i + 1, j, k, nx, ny, nz);
				
				// set corners for rotation
				corners[0] = getBccGridIndex(i + 1, j, k, nx, ny, nz);
				corners[1] = getBccGridIndex(i + 1, j, k + 1, nx, ny, nz);
				corners[2] = getBccGridIndex(i + 1, j + 1, k + 1, nx, ny, nz);
				corners[3] = getBccGridIndex(i + 1, j + 1, k, nx, ny, nz);
				
				// create four tets around the axis
				for (t = 0; t < 4; t++)
				{
					tet_memory[tet_index] = IndexTet(axis_a, axis_b, corners[t % 4], corners[(t + 1) % 4]);
					tet_index++;
				}
			}
		}
	}
}

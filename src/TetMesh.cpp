/*
WHY I WROTE THIS FILE:

I implemented the NON-INLINE functions of TetMesh.h.

by Alex Stuart 'Linearity'
*/

#include <assert.h>
#include <map>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include "IndexTet.h"
#include "slUtil.H"
#include "slVector.H"
#include "TetMesh.h"

// a set for face extraction
typedef std::tr1::unordered_set<SlTri, hashSlTri, eqSlTri> TriSet;

// tet orientation predicate from Shewchuk's "robust predicates" paper
double Orient3d(double *pa, double *pb, double *pc, double *pd)
{
	double adx, bdx, cdx;
	double ady, bdy, cdy;
	double adz, bdz, cdz;

	adx = pa[0] - pd[0];
	bdx = pb[0] - pd[0];
	cdx = pc[0] - pd[0];
	ady = pa[1] - pd[1];
	bdy = pb[1] - pd[1];
	cdy = pc[1] - pd[1];
	adz = pa[2] - pd[2];
	bdz = pb[2] - pd[2];
	cdz = pc[2] - pd[2];

	return 	  adx * (bdy * cdz - bdz * cdy)
		+ bdx * (cdy * adz - cdz * ady)
		+ cdx * (ady * bdz - adz * bdy);
}

static double triangleArea(SlVector3& v1, SlVector3& v2, SlVector3& v3)
{
	// Heron's formula
	double s1 = mag(v2 - v1), s2 = mag(v3 - v2), s3 = mag(v1 - v3);
	double sp = (s1 + s2 + s3) * 0.5;
	return sqrt(sp * (sp - s1) * (sp - s2) * (sp - s3));
}
/*
// NOTE: this is inefficient!
bool pointInTriangle(SlVector2 &point, SlVector2 &t1, SlVector2 &t2, SlVector2 &t3)
{
	return	(	   (    (cross(point - t1, t2 - t1) > 0)
			     && (cross(point - t2, t3 - t2) > 0)
			     && (cross(point - t3, t1 - t3) > 0) )
			|| (    (cross(point - t1, t2 - t1) <= 0)
			     && (cross(point - t2, t3 - t2) <= 0)
			     && (cross(point - t3, t1 - t3) <= 0) )	);
}

bool lineSegmentsIntersect(SlVector2 &s1, SlVector2 &s2, SlVector2 &t1, SlVector2 &t2)
{
	return	(	   (cross(s1 - t1, t2 - t1) * cross(s2 - t1, t2 - t1) < 0)
			&& (cross(t1 - s1, s2 - s1) * cross(t2 - s1, s2 - s1) < 0)	);
}

// NOTE: is this inefficient?
bool trianglesIntersect(SlVector2 &a, SlVector2 &b, SlVector2 &c, SlVector2 &d, SlVector2 &e, SlVector2 &f)
{
	return	(	   pointInTriangle(a, d, e, f)
			|| pointInTriangle(b, d, e, f)
			|| pointInTriangle(c, d, e, f)
			|| pointInTriangle(d, a, b, c)
			|| pointInTriangle(e, a, b, c)
			|| pointInTriangle(f, a, b, c)
			|| lineSegmentsIntersect(a, b, d, e)
			|| lineSegmentsIntersect(a, b, e, f)
			|| lineSegmentsIntersect(a, b, f, d)			
			|| lineSegmentsIntersect(b, c, d, e)
			|| lineSegmentsIntersect(b, c, e, f)
			|| lineSegmentsIntersect(b, c, f, d)
			|| lineSegmentsIntersect(c, a, d, e)
			|| lineSegmentsIntersect(c, a, e, f)
			|| lineSegmentsIntersect(c, a, f, d)	);
}
*/

TetMesh::TetMesh(SlVector3 *p_vertex_array, int p_vertex_count, IndexTet *p_tet_array, int p_tet_count) : vertex_array(p_vertex_array), vertex_count(p_vertex_count), marked_vertex_count(0), tet_array(p_tet_array), tet_count(p_tet_count), marked_tet_count(0)
{
	vertex_marker_array = new char[vertex_count];
	tet_marker_array = new char[tet_count];
	tet_circumcenter_array = new SlVector3[tet_count];
	tet_circumradius_array = new double[tet_count];
}

TetMesh::~TetMesh()
{
	delete[] vertex_marker_array;
	delete[] tet_marker_array;
	delete[] tet_circumcenter_array;
	delete[] tet_circumradius_array;
}

void TetMesh::computeCircumspheres()
{
	SlVector3 a, b, c, d;
	double sqr_mag_a, sqr_mag_b, sqr_mag_c, sqr_mag_d;
	double det_a_g, det_b_g, det_c_g, det_d_g;
	double det_a_x, det_b_x, det_c_x, det_d_x;
	double det_a_y, det_b_y, det_c_y, det_d_y;
	double det_a_z, det_b_z, det_c_z, det_d_z;
	double alpha, gamma, D_x, D_y, D_z;
	
	int i;
	for (i = 0; i < tet_count; i++)
	{
		a = *getVertexPointer((*getTetPointer(i))[0]);
		b = *getVertexPointer((*getTetPointer(i))[1]);
		c = *getVertexPointer((*getTetPointer(i))[2]);
		d = *getVertexPointer((*getTetPointer(i))[3]);
		
		sqr_mag_a = sqrMag(a);
		sqr_mag_b = sqrMag(b);
		sqr_mag_c = sqrMag(c);
		sqr_mag_d = sqrMag(d);
		
		det_a_g = b.x() * (c.y() * d.z() - d.y() * c.z()) - c.x() * (b.y() * d.z() - d.y() * b.z()) + d.x() * (b.y() * c.z() - c.y() * b.z());
		det_b_g = a.x() * (c.y() * d.z() - d.y() * c.z()) - c.x() * (a.y() * d.z() - d.y() * a.z()) + d.x() * (a.y() * c.z() - c.y() * a.z());
		det_c_g = a.x() * (b.y() * d.z() - d.y() * b.z()) - b.x() * (a.y() * d.z() - d.y() * a.z()) + d.x() * (a.y() * b.z() - b.y() * a.z());
		det_d_g = a.x() * (b.y() * c.z() - c.y() * b.z()) - b.x() * (a.y() * c.z() - c.y() * a.z()) + c.x() * (a.y() * b.z() - b.y() * a.z());
		
		det_a_x = (c.y() * d.z() - d.y() * c.z()) - (b.y() * d.z() - d.y() * b.z()) + (b.y() * c.z() - c.y() * b.z());
		det_b_x = (c.y() * d.z() - d.y() * c.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * c.z() - c.y() * a.z());
		det_c_x = (b.y() * d.z() - d.y() * b.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());
		det_d_x = (b.y() * c.z() - c.y() * b.z()) - (a.y() * c.z() - c.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());
		
		det_a_y = (c.x() * d.z() - d.x() * c.z()) - (b.x() * d.z() - d.x() * b.z()) + (b.x() * c.z() - c.x() * b.z());
		det_b_y = (c.x() * d.z() - d.x() * c.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * c.z() - c.x() * a.z());
		det_c_y = (b.x() * d.z() - d.x() * b.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());
		det_d_y = (b.x() * c.z() - c.x() * b.z()) - (a.x() * c.z() - c.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());
		
		det_a_z = (c.x() * d.y() - d.x() * c.y()) - (b.x() * d.y() - d.x() * b.y()) + (b.x() * c.y() - c.x() * b.y());
		det_b_z = (c.x() * d.y() - d.x() * c.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * c.y() - c.x() * a.y());
		det_c_z = (b.x() * d.y() - d.x() * b.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
		det_d_z = (b.x() * c.y() - c.x() * b.y()) - (a.x() * c.y() - c.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
		
		alpha = a.x() * det_a_x - b.x() * det_b_x + c.x() * det_c_x - d.x() * det_d_x;
		gamma = sqr_mag_a * det_a_g - sqr_mag_b * det_b_g + sqr_mag_c * det_c_g - sqr_mag_d * det_d_g;
		D_x = sqr_mag_a * det_a_x - sqr_mag_b * det_b_x + sqr_mag_c * det_c_x - sqr_mag_d * det_d_x;
		D_y = -1 * (sqr_mag_a * det_a_y - sqr_mag_b * det_b_y + sqr_mag_c * det_c_y - sqr_mag_d * det_d_y);
		D_z = sqr_mag_a * det_a_z - sqr_mag_b * det_b_z + sqr_mag_c * det_c_z - sqr_mag_d * det_d_z;
		
		computeCircumcenter(getTetPointer(i), D_x, D_y, D_z, alpha);
		computeCircumradius(getTetPointer(i), D_x, D_y, D_z, alpha, gamma);
	}
}

void TetMesh::computeCircumcenter(IndexTet *tet, double D_x, double D_y, double D_z, double alpha)
{
	tet_circumcenter_array[getTetIndex(tet)] = SlVector3(D_x, D_y, D_z) / (2 * alpha);
}

void TetMesh::computeCircumcenter(IndexTet *tet)
{
	SlVector3 a = *getVertexPointer((*tet)[0]);
	SlVector3 b = *getVertexPointer((*tet)[1]);
	SlVector3 c = *getVertexPointer((*tet)[2]);
	SlVector3 d = *getVertexPointer((*tet)[3]); 
	
	double sqr_mag_a = sqrMag(a);
	double sqr_mag_b = sqrMag(b);
	double sqr_mag_c = sqrMag(c);
	double sqr_mag_d = sqrMag(d);
	
	double det_a_x = (c.y() * d.z() - d.y() * c.z()) - (b.y() * d.z() - d.y() * b.z()) + (b.y() * c.z() - c.y() * b.z());
	double det_b_x = (c.y() * d.z() - d.y() * c.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * c.z() - c.y() * a.z());
	double det_c_x = (b.y() * d.z() - d.y() * b.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());
	double det_d_x = (b.y() * c.z() - c.y() * b.z()) - (a.y() * c.z() - c.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());

	double det_a_y = (c.x() * d.z() - d.x() * c.z()) - (b.x() * d.z() - d.x() * b.z()) + (b.x() * c.z() - c.x() * b.z());
	double det_b_y = (c.x() * d.z() - d.x() * c.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * c.z() - c.x() * a.z());
	double det_c_y = (b.x() * d.z() - d.x() * b.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());
	double det_d_y = (b.x() * c.z() - c.x() * b.z()) - (a.x() * c.z() - c.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());

	double det_a_z = (c.x() * d.y() - d.x() * c.y()) - (b.x() * d.y() - d.x() * b.y()) + (b.x() * c.y() - c.x() * b.y());
	double det_b_z = (c.x() * d.y() - d.x() * c.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * c.y() - c.x() * a.y());
	double det_c_z = (b.x() * d.y() - d.x() * b.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
	double det_d_z = (b.x() * c.y() - c.x() * b.y()) - (a.x() * c.y() - c.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
	
	double alpha = a.x() * det_a_x - b.x() * det_b_x + c.x() * det_c_x - d.x() * det_d_x;
	double D_x = sqr_mag_a * det_a_x - sqr_mag_b * det_b_x + sqr_mag_c * det_c_x - sqr_mag_d * det_d_x;
	double D_y = sqr_mag_a * det_a_y - sqr_mag_b * det_b_y + sqr_mag_c * det_c_y - sqr_mag_d * det_d_y;
	double D_z = sqr_mag_a * det_a_z - sqr_mag_b * det_b_z + sqr_mag_c * det_c_z - sqr_mag_d * det_d_z;
	
	computeCircumcenter(tet, D_x, D_y, D_z, alpha);
}

SlVector3 TetMesh::getCircumcenter(IndexTet *tet)
{
	return tet_circumcenter_array[getTetIndex(tet)];
}

void TetMesh::computeCircumradius(IndexTet *tet, double D_x, double D_y, double D_z, double alpha, double gamma)
{
	tet_circumradius_array[getTetIndex(tet)] = sqrt(D_x * D_x + D_y * D_y + D_z * D_z - 4 * alpha * gamma) / (2 * fabs(alpha));
}

void TetMesh::computeCircumradius(IndexTet *tet)
{
	SlVector3 a = *getVertexPointer((*tet)[0]);
	SlVector3 b = *getVertexPointer((*tet)[1]);
	SlVector3 c = *getVertexPointer((*tet)[2]);
	SlVector3 d = *getVertexPointer((*tet)[3]);
	
	double sqr_mag_a = sqrMag(a);
	double sqr_mag_b = sqrMag(b);
	double sqr_mag_c = sqrMag(c);
	double sqr_mag_d = sqrMag(d);
	
	double det_a_g = b.x() * (c.y() * d.z() - d.y() * c.z()) - c.x() * (b.y() * d.z() - d.y() * b.z()) + d.x() * (b.y() * c.z() - c.y() * b.z());
	double det_b_g = a.x() * (c.y() * d.z() - d.y() * c.z()) - c.x() * (a.y() * d.z() - d.y() * a.z()) + d.x() * (a.y() * c.z() - c.y() * a.z());
	double det_c_g = a.x() * (b.y() * d.z() - d.y() * b.z()) - b.x() * (a.y() * d.z() - d.y() * a.z()) + d.x() * (a.y() * b.z() - b.y() * a.z());
	double det_d_g = a.x() * (b.y() * c.z() - c.y() * b.z()) - b.x() * (a.y() * c.z() - c.y() * a.z()) + c.x() * (a.y() * b.z() - b.y() * a.z());
	
	double det_a_x = (c.y() * d.z() - d.y() * c.z()) - (b.y() * d.z() - d.y() * b.z()) + (b.y() * c.z() - c.y() * b.z());
	double det_b_x = (c.y() * d.z() - d.y() * c.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * c.z() - c.y() * a.z());
	double det_c_x = (b.y() * d.z() - d.y() * b.z()) - (a.y() * d.z() - d.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());
	double det_d_x = (b.y() * c.z() - c.y() * b.z()) - (a.y() * c.z() - c.y() * a.z()) + (a.y() * b.z() - b.y() * a.z());

	double det_a_y = (c.x() * d.z() - d.x() * c.z()) - (b.x() * d.z() - d.x() * b.z()) + (b.x() * c.z() - c.x() * b.z());
	double det_b_y = (c.x() * d.z() - d.x() * c.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * c.z() - c.x() * a.z());
	double det_c_y = (b.x() * d.z() - d.x() * b.z()) - (a.x() * d.z() - d.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());
	double det_d_y = (b.x() * c.z() - c.x() * b.z()) - (a.x() * c.z() - c.x() * a.z()) + (a.x() * b.z() - b.x() * a.z());

	double det_a_z = (c.x() * d.y() - d.x() * c.y()) - (b.x() * d.y() - d.x() * b.y()) + (b.x() * c.y() - c.x() * b.y());
	double det_b_z = (c.x() * d.y() - d.x() * c.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * c.y() - c.x() * a.y());
	double det_c_z = (b.x() * d.y() - d.x() * b.y()) - (a.x() * d.y() - d.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
	double det_d_z = (b.x() * c.y() - c.x() * b.y()) - (a.x() * c.y() - c.x() * a.y()) + (a.x() * b.y() - b.x() * a.y());
	
	double alpha = a.x() * det_a_x - b.x() * det_b_x + c.x() * det_c_x - d.x() * det_d_x;
	double gamma = sqr_mag_a * det_a_g - sqr_mag_b * det_b_g + sqr_mag_c * det_c_g - sqr_mag_d * det_d_g;
	double D_x = sqr_mag_a * det_a_x - sqr_mag_b * det_b_x + sqr_mag_c * det_c_x - sqr_mag_d * det_d_x;
	double D_y = -1 * (sqr_mag_a * det_a_y - sqr_mag_b * det_b_y + sqr_mag_c * det_c_y - sqr_mag_d * det_d_y);
	double D_z = sqr_mag_a * det_a_z - sqr_mag_b * det_b_z + sqr_mag_c * det_c_z - sqr_mag_d * det_d_z;
	
	computeCircumradius(tet, D_x, D_y, D_z, alpha, gamma);
}

double TetMesh::getCircumradius(IndexTet *tet)
{
	return tet_circumradius_array[getTetIndex(tet)];
}

double TetMesh::getVolume(IndexTet *tet)
{
	return fabs(dot(*getVertexPointer((*tet)[0]) - *getVertexPointer((*tet)[3]), cross(*getVertexPointer((*tet)[1]) - *getVertexPointer((*tet)[3]), *getVertexPointer((*tet)[2]) - *getVertexPointer((*tet)[3])))) / 6;
}

/*
double TetMesh::getDihedralAngle(IndexTet *tet, int vertex_a_index, int vertex_b_index)
{
	int min, max;
	SlVector3 a, b, c, d;
	double sine;
	
	if (vertex_a_index < vertex_b_index)
	{
		min = vertex_a_index;
		max = vertex_b_index;
	}
	else
	{
		min = vertex_b_index;
		max = vertex_a_index;
	}
	
	a = *getVertexPointer((*tet)[vertex_a_index]);
	b = *getVertexPointer((*tet)[vertex_b_index]);
	
	if (max - min == 1)
	{
		c = *getVertexPointer((*tet)[(min + 4 - 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 + 1) % 4]);
	}
	else if (max - min == 2)
	{
		c = *getVertexPointer((*tet)[(min + 4 + 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 + 1) % 4]);
	}
	else
	{
		c = *getVertexPointer((*tet)[(min + 4 + 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 - 1) % 4]);
	}
	
	sine = getVolume(tet) * 3 * mag(b - a) / (2 * triangleArea(a, b, c) * triangleArea(a, b, d));
	
	return asin(((fabs(sine) > 1.0) ? (((sine > 0) ? (1.0) : (-1.0))) : (sine)));
}
*/

double TetMesh::getDihedralAngle(IndexTet *tet, int vertex_a_index, int vertex_b_index)
{
	int min, max;
	SlVector3 a, b, c, d;
	
	if (vertex_a_index < vertex_b_index)
	{
		min = vertex_a_index;
		max = vertex_b_index;
	}
	else
	{
		min = vertex_b_index;
		max = vertex_a_index;
	}
	
	a = *getVertexPointer((*tet)[vertex_a_index]);
	b = *getVertexPointer((*tet)[vertex_b_index]);
	
	if (max - min == 1)
	{
		c = *getVertexPointer((*tet)[(min + 4 - 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 + 1) % 4]);
	}
	else if (max - min == 2)
	{
		c = *getVertexPointer((*tet)[(min + 4 + 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 + 1) % 4]);
	}
	else
	{
		c = *getVertexPointer((*tet)[(min + 4 + 1) % 4]);
		d = *getVertexPointer((*tet)[(max + 4 - 1) % 4]);
	}
	
	SlVector3 normal_c = cross(c - a, b - a);
	SlVector3 normal_d = cross(d - b, a - b);
	
	return M_PI - acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
}

double TetMesh::getMinDihedralAngle(IndexTet *tet)
{
	//assert(orient3d(tet) > 0);
	int i, j;
	SlVector3 a, b, c, d, normal_c, normal_d;
	
	double min_da = M_PI, current_da = 0;
	
	a = *getVertexPointer((*tet)[0]);
	b = *getVertexPointer((*tet)[1]);
	c = *getVertexPointer((*tet)[2]);
	d = *getVertexPointer((*tet)[3]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	a = *getVertexPointer((*tet)[1]);
	b = *getVertexPointer((*tet)[2]);
	c = *getVertexPointer((*tet)[0]);
	d = *getVertexPointer((*tet)[3]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	a = *getVertexPointer((*tet)[2]);
	b = *getVertexPointer((*tet)[3]);
	c = *getVertexPointer((*tet)[0]);
	d = *getVertexPointer((*tet)[1]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	a = *getVertexPointer((*tet)[3]);
	b = *getVertexPointer((*tet)[0]);
	c = *getVertexPointer((*tet)[2]);
	d = *getVertexPointer((*tet)[1]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	a = *getVertexPointer((*tet)[0]);
	b = *getVertexPointer((*tet)[2]);
	c = *getVertexPointer((*tet)[3]);
	d = *getVertexPointer((*tet)[1]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	a = *getVertexPointer((*tet)[3]);
	b = *getVertexPointer((*tet)[1]);
	c = *getVertexPointer((*tet)[0]);
	d = *getVertexPointer((*tet)[2]);
	normal_c = cross(c - a, b - a);
	normal_d = cross(d - b, a - b);
	current_da = acos(dot(normal_c / mag(normal_c), normal_d / mag(normal_d)));
	min_da = ((current_da < min_da) ? (current_da) : min_da);
	
	return M_PI - min_da;
}

SlVector3 TetMesh::getPseudoNormal(IndexTet *tet, int vertex_a_index, int vertex_b_index)
{
	int min, max;
	int vertex_c_index, vertex_d_index;
	double sine;
	
	if (vertex_a_index < vertex_b_index)
	{
		min = vertex_a_index;
		max = vertex_b_index;
	}
	else
	{
		min = vertex_b_index;
		max = vertex_a_index;
	}
		
	if (max - min == 1)
	{
		vertex_c_index = (min + 4 - 1) % 4;
		vertex_d_index = (max + 4 + 1) % 4;
	}
	else if (max - min == 2)
	{
		vertex_c_index = (min + 4 + 1) % 4;
		vertex_d_index = (max + 4 + 1) % 4;
	}
	else
	{
		vertex_c_index = (min + 4 + 1) % 4;
		vertex_d_index = (max + 4 - 1) % 4;
	}
	
	SlVector3 normal_abc, normal_abd;
	normal_abc = cross(	*getVertexPointer((*tet)[(((vertex_d_index + 1) % 2) == 0) ? ((vertex_d_index + 2) % 4) : ((vertex_d_index + 3) % 4)]) - *getVertexPointer((*tet)[(vertex_d_index + 1) % 4]),
				*getVertexPointer((*tet)[(((vertex_d_index + 1) % 2) == 0) ? ((vertex_d_index + 3) % 4) : ((vertex_d_index + 2) % 4)]) - *getVertexPointer((*tet)[(vertex_d_index + 1) % 4])	);
	normal_abd = cross(	*getVertexPointer((*tet)[(((vertex_c_index + 1) % 2) == 0) ? ((vertex_c_index + 2) % 4) : ((vertex_c_index + 3) % 4)]) - *getVertexPointer((*tet)[(vertex_c_index + 1) % 4]),
				*getVertexPointer((*tet)[(((vertex_c_index + 1) % 2) == 0) ? ((vertex_c_index + 3) % 4) : ((vertex_c_index + 2) % 4)]) - *getVertexPointer((*tet)[(vertex_c_index + 1) % 4])	);
	
	normalize(normal_abc);
	normalize(normal_abd);
	
	normal_abc = normal_abc + normal_abd;
	normalize(normal_abc);
	
	return normal_abc;
}

void TetMesh::reorientTets()
{
	int i;
	int temp_index;
	for ( i = 0; i < tet_count; i++ )
	{
		if ( orient3d( &tet_array[i] ) < 0.0 )
		{
			temp_index = tet_array[i][2];
			tet_array[i][2] = tet_array[i][3];
			tet_array[i][3] = temp_index;
		}
		assert( orient3d( &tet_array[i] ) >= 0.0 );
	}
}

bool TetMesh::pointInTet(SlVector3 point, IndexTet *tet)
{
	int i;
	SlVector3 a, b, n, reference, compared;
	
	for (i = 0; i < 4; i++)
	{
		// the vectors defining the current face of the tetrahedron
		a = *getVertexPointer((*tet)[(i + 1) % 4]) - *getVertexPointer((*tet)[i]);
		b = *getVertexPointer((*tet)[(i + 2) % 4]) - *getVertexPointer((*tet)[i]);
		
		reference = *getVertexPointer((*tet)[i % 4]);
		compared = *getVertexPointer((*tet)[(i + 3) % 4]);
		
		// normalized cross product
		// NOTE: depending on how the tet is oriented, the normals may be all the wrong direction
		
		n = cross(a, b);
		n *= pow(-1.0, i) / mag(cross(a, b));
		
		if ((dot(point - reference, n) > 0.0) != (dot(compared - reference, n) > 0.0))
		{
			return false;
		}
	}
	
	return true;
}

// Orient3d wrapper that isn't so Goddamn high-maintenance
double TetMesh::orient3d(IndexTet *tet)
{
	return Orient3d(	getVertexPointer((*tet)[0])->data,
				getVertexPointer((*tet)[1])->data,
				getVertexPointer((*tet)[2])->data,
				getVertexPointer((*tet)[3])->data	);
}

/*
bool TetMesh::triIntersectsTet(SlVector3 &a, SlVector3 &b, SlVector3 &c, IndexTet *tet)
{
	SlVector3 tri_normal = cross(b - a, c - a);
	tri_normal = tri_normal / mag(tri_normal);
	
	// on which sides of triangle plane do the tet vertices lie?
	double vertex_sides[4];
	vertex_sides[0] = dot(*getVertexPointer((*tet)[0]) - a, tri_normal);
	vertex_sides[1] = dot(*getVertexPointer((*tet)[1]) - a, tri_normal);
	vertex_sides[2] = dot(*getVertexPointer((*tet)[2]) - a, tri_normal);
	vertex_sides[3] = dot(*getVertexPointer((*tet)[3]) - a, tri_normal);
	
	// possible intersection points
	SlVector3 intersections[4];
	
	int i = 0;
	int j = 0;
	int k = 0;
	// for all pairs of tet vertices on opposite sides (maximum of 4 pairs)
	while ((k < 4) && (i < 3))
	{
		for (j = i + 1; j < 4; j++)
		{
			if (vertex_sides[i] * vertex_sides[j] < 0)
			{
				// compute intersection of tet edge and triangle
				intersections[k] = *getVertexPointer((*tet)[i]) + (dot(tri_normal, a) - dot(tri_normal, *getVertexPointer((*tet)[i]))) / dot(tri_normal, *getVertexPointer((*tet)[j]) - *getVertexPointer((*tet)[i])) * (*getVertexPointer((*tet)[j]) - *getVertexPointer((*tet)[i]));
				k++;
			}
		}
		i++;
	}
	
	// project everything to triangle plane and convert to barycentric coordinates
	SlVector2 a_prime, b_prime, c_prime;
	SlVector2 intersections_prime[4];
	
	a_prime[0] = 0;
	a_prime[1] = 0;
	
	b_prime[0] = 1;
	b_prime[1] = 0;
	
	c_prime[0] = 0;
	c_prime[0] = 1;
	
	switch (k)
	{
	case 0:
		return false;
		break;
		
	case 3:
		return trianglesIntersect(intersections[0], intersections[1], intersections[2], a, b, c);
		break;
		
	case 4:
		return	(    trianglesIntersect(intersections[0], intersections[1], intersections[2], a, b, c)
			  || trianglesIntersect(intersections[0], intersections[2], intersections[3], a, b, c) );
		break;
	default:
		// this should never happen
		assert(false);
	}
}
*/

void TetMesh::removeMarkedVertices()
{
	int new_vertex_indices[vertex_count];
	
	int i, j;
	
	// initialize new vertex indices with old vertex indices
	for (i = 0; i < vertex_count; i++)
	{
		new_vertex_indices[i] = i;
	}
	
	// loop invariant: every array location before index i is final
	for (i = 0; i < vertex_count; i++)
	{
		// if we find a marked element
		if (vertex_marker_array[i])
		{
			// search forward for an unmarked element
			for (j = i + 1; (j < vertex_count) && vertex_marker_array[j]; j++) {}
			// if we find one
			if (j < vertex_count)
			{
				// copy it to the location of the marked element
				vertex_array[i] = vertex_array[j];
				// mark its old location
				vertex_marker_array[j] = 1;
				// and map the old location to the new one
				new_vertex_indices[j] = i;
			}
			// if we don't find one
			else
			{
				// then we are done, and we have i vertices
				vertex_count = i;
				break;
			}
		}
	}
	
	for (i = 0; i < tet_count; i++)
	{
		assert(new_vertex_indices[(*getTetPointer(i))[0]] < vertex_count);
		assert(new_vertex_indices[(*getTetPointer(i))[1]] < vertex_count);
		assert(new_vertex_indices[(*getTetPointer(i))[2]] < vertex_count);
		assert(new_vertex_indices[(*getTetPointer(i))[3]] < vertex_count);
		
		(*getTetPointer(i))[0] = new_vertex_indices[(*getTetPointer(i))[0]];
		(*getTetPointer(i))[1] = new_vertex_indices[(*getTetPointer(i))[1]];
		(*getTetPointer(i))[2] = new_vertex_indices[(*getTetPointer(i))[2]];
		(*getTetPointer(i))[3] = new_vertex_indices[(*getTetPointer(i))[3]];
	}
}

void TetMesh::removeMarkedTets()
{
	int i, j;
	// loop invariant: every array location before index i is final
	for (i = 0; i < tet_count; i++)
	{
		// if we find a marked element
		if (tet_marker_array[i])
		{
			// search forward for an unmarked element
			for (j = i + 1; (j < tet_count) && tet_marker_array[j]; j++) {}
			// if we find one
			if (j < tet_count)
			{
				// copy it to the location of the marked element
				tet_array[i] = tet_array[j];
				// and mark its old location
				tet_marker_array[j] = 1;
			}
			// if we don't find one
			else
			{
				// then we are done, and we have i tets
				tet_count = i;
				break;
			}
		}
	}
}

void TetMesh::computeOneRing(int index)
{
	vertex_one_ring_map.erase(index);
	
	clearVertexMarks();
	
	int i, j;
	for (i = 0; i < tet_count; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (tet_array[i][j] == index)
			{
				markVertex(tet_array[i][0]);
				markVertex(tet_array[i][1]);
				markVertex(tet_array[i][2]);
				markVertex(tet_array[i][3]);
				break;
			}
		}
	}
	
	for (i = 0; i < vertex_count; i++)
	{
		if (vertexMarked(i))
		{
			vertex_one_ring_map.insert(std::tr1::unordered_map<int, int>::value_type(index, i));
		}
	}
}

int TetMesh::sizeOfOneRing(int index)
{
	if (vertex_one_ring_map.count(index) == 0)
	{
		computeOneRing(index);
	}
	
	return vertex_one_ring_map.count(index);
}

void TetMesh::getOneRing(int index, int *one_ring_index_array)
{
	std::pair<std::tr1::unordered_multimap<int, int>::iterator, std::tr1::unordered_multimap<int, int>::iterator> iterPair;
	
	if (vertex_one_ring_map.count(index) == 0)
	{
		computeOneRing(index);
	}
	
	iterPair = vertex_one_ring_map.equal_range(index);
	
	int i = 0;
	for (std::tr1::unordered_multimap<int, int>::iterator vertIter = iterPair.first; vertIter != iterPair.second; vertIter++)
	{
		one_ring_index_array[i] = vertIter->second;
		i++;
	}
}

void TetMesh::computeIncidentTets(SlVector3 *vertex)
{
	int vertex_index = getVertexIndex(vertex);
	
	vertex_incident_tets_map.erase(vertex_index);
	
	int i, j;
	for (i = 0; i < tet_count; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (tet_array[i][j] == vertex_index)
			{
				vertex_incident_tets_map.insert(std::tr1::unordered_map<int, int>::value_type(vertex_index, i));
				break;
			}
		}
	}
}

int TetMesh::numberOfIncidentTets(SlVector3 *vertex)
{
	if (vertex_incident_tets_map.count(getVertexIndex(vertex)) == 0)
	{
		computeIncidentTets(vertex);
	}
	
	return vertex_incident_tets_map.count(getVertexIndex(vertex));
}

void TetMesh::getIncidentTets(SlVector3 *vertex, int *incident_tets_index_array)
{
	std::pair<std::tr1::unordered_multimap<int, int>::iterator, std::tr1::unordered_multimap<int, int>::iterator> iterPair;
	
	if (vertex_incident_tets_map.count(getVertexIndex(vertex)) == 0)
	{
		computeIncidentTets(vertex);
	}
	
	iterPair = vertex_incident_tets_map.equal_range(getVertexIndex(vertex));
	
	int i = 0;
	for (std::tr1::unordered_multimap<int, int>::iterator vertIter = iterPair.first; vertIter != iterPair.second; vertIter++)
	{
		incident_tets_index_array[i] = vertIter->second;
		i++;
	}
}

void TetMesh::computeAdjacentTets()
{
	if (triangle_tet_map.empty())
	{
		mapTrianglesToTets();
	}                       
	
	TriToTetMap::iterator oldIter;
	TriToTetMap::iterator newIter;
	for (TriToTetMap::iterator keyValueIter = triangle_tet_map.begin(); keyValueIter != triangle_tet_map.end(); keyValueIter++)
	{
		if (triangle_tet_map.count(keyValueIter->first) == 2)
		{
			oldIter = keyValueIter;
			newIter = ++keyValueIter;
			tet_adjacent_tets_map.insert(std::tr1::unordered_multimap<int, int>::value_type(getTetIndex(oldIter->second), getTetIndex(newIter->second)));
			tet_adjacent_tets_map.insert(std::tr1::unordered_multimap<int, int>::value_type(getTetIndex(newIter->second), getTetIndex(oldIter->second)));
		}
	}
}

int TetMesh::numberOfAdjacentTets(IndexTet *tet)
{
	if (tet_adjacent_tets_map.empty())
	{
		computeAdjacentTets();
	}
	
	return tet_adjacent_tets_map.count(getTetIndex(tet));
}

void TetMesh::getAdjacentTets(IndexTet *tet, int *adjacent_tets_index_array)
{
	std::pair<std::tr1::unordered_multimap<int, int>::iterator, std::tr1::unordered_multimap<int, int>::iterator> tet_iter_pair;
	std::tr1::unordered_multimap<int, int>::iterator tet_iter;
	tet_iter_pair = tet_adjacent_tets_map.equal_range(getTetIndex(tet));
	
	int i = 0;
	for (tet_iter = tet_iter_pair.first; tet_iter != tet_iter_pair.second; tet_iter++)
	{
		adjacent_tets_index_array[i] = tet_iter->second;
		i++;
	}
}

void TetMesh::mapTrianglesToTets()
{
	int i, j;
	SlTri currentTri;
	
	triangle_tet_map.clear();
	for (i = 0; i < tet_count; i++)
	{
		if (orient3d(getTetPointer(i)) > 0)
		{
			for (int j = 0; j < 4; j++)
			{
				// orient triangle outward by right-hand rule
				// this assumes a "right-handed" tet orientation
				currentTri[0] = (*getTetPointer(i))[j];
				currentTri[1] = (*getTetPointer(i))[((j % 2) == 0)?((j + 1) % 4):((j + 2) % 4)];
				currentTri[2] = (*getTetPointer(i))[((j % 2) == 0)?((j + 2) % 4):((j + 1) % 4)];
	
				triangle_tet_map.insert(TriToTetMap::value_type(currentTri, getTetPointer(i)));
			}
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				// orient triangle outward by right-hand rule
				// this assumes a "left-handed" tet orientation
				currentTri[0] = (*getTetPointer(i))[j];
				currentTri[1] = (*getTetPointer(i))[((j % 2) == 0)?((j + 2) % 4):((j + 1) % 4)];
				currentTri[2] = (*getTetPointer(i))[((j % 2) == 0)?((j + 1) % 4):((j + 2) % 4)];
	
				triangle_tet_map.insert(TriToTetMap::value_type(currentTri, getTetPointer(i)));
			}
		}
	}
}

void TetMesh::extractFaces(SlTri *faces, int *face_count)
{
	int i, j;
	SlTri currentTri;
	TriSet faceSet;
	
	for (i = 0; i < tet_count; i++)
	{
		if (orient3d(getTetPointer(i)) > 0)
		{
			for (int j = 0; j < 4; j++)
			{
				// orient triangle outward by right-hand rule
				// this assumes a "right-handed" tet orientation
				currentTri[0] = (*getTetPointer(i))[j];
				currentTri[1] = (*getTetPointer(i))[((j % 2) == 0)?((j + 1) % 4):((j + 2) % 4)];
				currentTri[2] = (*getTetPointer(i))[((j % 2) == 0)?((j + 2) % 4):((j + 1) % 4)];
	
				faceSet.insert(currentTri);
			}
		}
		else
		{
			for (int j = 0; j < 4; j++)
			{
				// orient triangle outward by right-hand rule
				// this assumes a "left-handed" tet orientation
				currentTri[0] = (*getTetPointer(i))[j];
				currentTri[1] = (*getTetPointer(i))[((j % 2) == 0)?((j + 2) % 4):((j + 1) % 4)];
				currentTri[2] = (*getTetPointer(i))[((j % 2) == 0)?((j + 1) % 4):((j + 2) % 4)];
	
				faceSet.insert(currentTri);
			}
		}
	}
	
	i = 0;
	for (TriSet::iterator triIter = faceSet.begin(); triIter != faceSet.end(); triIter++)
	{
		faces[i] = *triIter;
		i++;
	}
	*face_count = i;
}

void TetMesh::extractSurface(SlVector3 *surface_vertices, int *surface_vertex_count, SlTri *surface_triangles, int *surface_triangle_count)
{
	int i, j;
	SlTri currentTri;
	
	mapTrianglesToTets();
	
	// separate the triangles incident to only one tet, mark their vertices
	clearVertexMarks();
	i = 0;
	for (TriToTetMap::iterator keyValueIter = triangle_tet_map.begin(); keyValueIter != triangle_tet_map.end(); keyValueIter++)
	{
		if (triangle_tet_map.count(keyValueIter->first) == 1)
		{
			surface_triangles[i] = keyValueIter->first;
			markVertex(surface_triangles[i][0]);
			markVertex(surface_triangles[i][1]);
			markVertex(surface_triangles[i][2]);
			i++;
		}
	}
	*surface_triangle_count = i;
	
	// separate the marked vertices, map their indices to indices in the surface vertex array
	surface_index_map.clear();
	j = 0;
	for (i = 0; i < vertex_count; i++)
	{
		if (vertex_marker_array[i])
		{
			surface_vertices[j] = vertex_array[i];
			surface_index_map.insert(std::tr1::unordered_map<int, int>::value_type(i, j));
			j++;
		}	
	}
	*surface_vertex_count = j;
	
	// translate each triangle so it refers to the surface vertex array
	for (i = 0; i < *surface_triangle_count; i++)
	{
		for (j = 0; j < 3; j++)
		{
			surface_triangles[i][j] = surface_index_map[surface_triangles[i][j]];
		}
	}
	
	// create inverse surface index map
	inverse_surface_index_map.clear();
	for (std::tr1::unordered_map<int, int>::iterator pairIter = surface_index_map.begin(); pairIter != surface_index_map.end(); pairIter++)
	{
		inverse_surface_index_map.insert(std::tr1::unordered_map<int, int>::value_type((*pairIter).second, (*pairIter).first));
	}
}

void TetMesh::updateSurface(SlVector3 *surface_vertices, int surface_vertex_count, SlTri *surface_triangles, int surface_triangle_count)
{
	// NOTE: this function does not (yet?) use the triangle data parameters
	int i;
	
	if (inverse_surface_index_map.empty())
	{
		fprintf(stderr, "(updating surface) error: surface not updated because surface was never extracted.\n");
		return;
	}
	
	for (i = 0; i < surface_vertex_count; i++)
	{
		vertex_array[inverse_surface_index_map[i]] = surface_vertices[i];
	}
}

bool TetMesh::isSurfaceVertex(int index)
{
	if (inverse_surface_index_map.empty())
	{
		fprintf(stderr, "(checking for surface vertex) error: whether vertex is on the surface not determined because surface was never extracted.\n");
		return false;
	}
	
	return (surface_index_map.count(index) != 0);
}

bool TetMesh::isSurfaceFace(SlTri face)
{
	if (triangle_tet_map.empty())
	{
		fprintf(stderr, "(checking for surface face) error: whether face is on the surface not determined because surface was never extracted.\n");
		return false;
	}
	
	return (triangle_tet_map.count(face) == 1);
}

int TetMesh::all2SurfaceIndex(int index)
{
	if (surface_index_map.empty())
	{
		fprintf(stderr, "(converting index to surface indices) error: index not converted to surface index because surface was never extracted.\n");
		return -1;
	}
	
	return surface_index_map[index];
}

int TetMesh::surface2AllIndex(int index)
{
	if (inverse_surface_index_map.empty())
	{
		fprintf(stderr, "(converting index from surface indices) error: surface index not converted because surface was never extracted.\n");
		return -1;
	}
	
	return inverse_surface_index_map[index];
}

#ifndef __TET_MESH__
#define __TET_MESH__

/*
WHY I WROTE THIS FILE:

I wanted a way to maintain tet mesh data coherence while allowing convenient
access to those data.

by Alex Stuart 'Linearity'
*/

#include <tr1/unordered_map>

#include "IndexTet.h"
#include "slUtil.H"
#include "slVector.H"

// a hash function for SlTri objects
struct hashSlTri {
  size_t operator()(const SlTri& t) const {
    return (t[0] ^ t[1] ^ t[2]);
  }
};

// a comparator for SlTri objects
struct eqSlTri {
  bool operator()(const SlTri &t1, const SlTri &t2) const {
    return ((t1[0] == t2[0] && t1[1] == t2[1] && t1[2] == t2[2]) ||
	    (t1[1] == t2[0] && t1[0] == t2[1] && t1[2] == t2[2]) ||
	    (t1[2] == t2[0] && t1[1] == t2[1] && t1[0] == t2[2]) ||
	    (t1[0] == t2[0] && t1[2] == t2[1] && t1[1] == t2[2]) ||
	    (t1[1] == t2[0] && t1[2] == t2[1] && t1[0] == t2[2]) ||
	    (t1[2] == t2[0] && t1[0] == t2[1] && t1[1] == t2[2]));
  }
};

// an associative array for surface extraction
typedef std::tr1::unordered_multimap<SlTri, IndexTet *, hashSlTri, eqSlTri> TriToTetMap;

class TetMesh
{
public:
	TetMesh(SlVector3 *p_vertex_array = NULL, int p_vertex_count = 0, IndexTet *p_tet_array = NULL, int p_tet_count = 0);
	~TetMesh();
	
	// data retrieval
	inline SlVector3 *getVertexPointer(int index);
	inline int getVertexIndex(SlVector3 *pointer);
	inline IndexTet *getTetPointer(int index);
	inline int getTetIndex(IndexTet *pointer);
	
	// tet properties
	void computeCircumspheres();
	void computeCircumcenter(IndexTet *tet, double D_x, double D_y, double D_z, double alpha);
	void computeCircumcenter(IndexTet *tet);
	SlVector3 getCircumcenter(IndexTet *tet);
	void computeCircumradius(IndexTet *tet, double D_x, double D_y, double D_z, double alpha, double gamma);
	void computeCircumradius(IndexTet *tet);
	double getCircumradius(IndexTet *tet);
	double getVolume(IndexTet *tet);
	double getDihedralAngle(IndexTet *tet, int vertex_a_index, int vertex_b_index);
	double getMinDihedralAngle(IndexTet *tet);
	SlVector3 getPseudoNormal(IndexTet *tet, int vertex_a_index, int vertex_b_index);
	void reorientTets();
	
	// tests
	bool pointInTet(SlVector3 point, IndexTet *tet);
	double orient3d(IndexTet *tet);
	//bool triIntersectsTet(SlVector3 &a, SlVector3 &b, SlVector3 &c, IndexTet *tet);
	
	// marking and culling
	inline void markVertex(int index);
	inline void unmarkVertex(int index);
	inline void clearVertexMarks();
	inline bool vertexMarked(int index);
	void removeMarkedVertices();
	inline void markTet(int index);
	inline void unmarkTet(int index);
	inline void clearTetMarks();
	inline bool tetMarked(int index);
	void removeMarkedTets();
	
	// connectivity
	void computeOneRing(int index);	// computes vertex one-ring, stores in map
	int sizeOfOneRing(int index);	// returns size of one-ring
	void getOneRing(int index, int *one_ring_index_array);	// copies one-ring to array
	void computeIncidentTets(SlVector3 *vertex);	// computes incident tets, stores in map
	int numberOfIncidentTets(SlVector3 *vertex);	// returns number of incident tets
	void getIncidentTets(SlVector3 *vertex, int *incident_tets_index_array);	// copies incident tets to array
	void computeAdjacentTets();	// computes adjacent tets, stores in map
	int numberOfAdjacentTets(IndexTet *tet);	// returns number of adjacent tets
	void getAdjacentTets(IndexTet *tet, int *adjacent_tets_index_array);	// copies adjacent tets to array
	
	// face retrieval
	void mapTrianglesToTets();	// maps triangles to tets sharing them as faces
	void extractFaces(SlTri *faces, int *face_count);
	
	// surface manipulation
	void extractSurface(SlVector3 *surface_vertices, int *surface_vertex_count, SlTri *surface_triangles, int *surface_triangle_count);
	void updateSurface(SlVector3 *surface_vertices, int surface_vertex_count, SlTri *surface_triangles, int surface_triangle_count);
	bool isSurfaceVertex(int index);
	bool isSurfaceFace(SlTri face);
	int all2SurfaceIndex(int index); // map general index to surface index
	int surface2AllIndex(int index); // map surface index to general index

	SlVector3 *vertex_array;		// provided externally
	char *vertex_marker_array;		// heap-allocated on instantiation
	int vertex_count;
	int marked_vertex_count;

	IndexTet *tet_array;			// provided externally
	char *tet_marker_array;			// heap-allocated on instantiation
	SlVector3 *tet_circumcenter_array;	// heap-allocated on instantiation
	double *tet_circumradius_array;		// heap-allocated on instantiation
	int tet_count;
	int marked_tet_count;
	
	// mapping of triangles to tetrahedra
	TriToTetMap triangle_tet_map;
	
	// mappings for getting surface triangle indices correct
	std::tr1::unordered_map<int, int> surface_index_map; // all to surface
	std::tr1::unordered_map<int, int> inverse_surface_index_map; // surface to all
	
	// mapping of vertices to their one-rings
	std::tr1::unordered_multimap<int, int> vertex_one_ring_map;
	
	// mapping of vertices to their incident tets
	std::tr1::unordered_multimap<int, int> vertex_incident_tets_map;
	
	// mapping of tetrahedra to their adjacent tets
	std::tr1::unordered_multimap<int, int> tet_adjacent_tets_map;
};

inline SlVector3 *TetMesh::getVertexPointer(int index)
{
	return &(vertex_array[index]);
}

inline int TetMesh::getVertexIndex(SlVector3 *pointer)
{
	return pointer - vertex_array;
}

inline IndexTet *TetMesh::getTetPointer(int index)
{
	return &(tet_array[index]);
}

inline int TetMesh::getTetIndex(IndexTet *pointer)
{
	return pointer - tet_array;
}

inline void TetMesh::markVertex(int index)
{
	if (vertex_marker_array[index] == 0)
		marked_vertex_count++;
	
	vertex_marker_array[index] = 1;
}

inline void TetMesh::unmarkVertex(int index)
{
	if (vertex_marker_array[index] == 1)
		marked_vertex_count--;
	
	vertex_marker_array[index] = 0;
}

inline void TetMesh::clearVertexMarks()
{
	marked_vertex_count = 0;
	
	int i;
	for (i = 0; i < vertex_count; i++)
	{
		unmarkVertex(i);
	}
}

inline bool TetMesh::vertexMarked(int index)
{
	return vertex_marker_array[index] == 1;
}

inline void TetMesh::markTet(int index)
{
	if (tet_marker_array[index] == 0)
		marked_tet_count++;
	
	tet_marker_array[index] = 1;
}

inline void TetMesh::unmarkTet(int index)
{
	if (tet_marker_array[index] == 0)
		marked_tet_count++;
	
	tet_marker_array[index] = 0;
}

inline void TetMesh::clearTetMarks()
{
	marked_tet_count = 0;
	
	int i;
	for (i = 0; i < tet_count; i++)
	{
		unmarkTet(i);
	}
}

inline bool TetMesh::tetMarked(int index)
{
	return tet_marker_array[index] == 1;
}

#endif

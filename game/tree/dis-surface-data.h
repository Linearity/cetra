/* target surface display */

#include <slUtil.H>
#include <slVector.H>
#include <vector>

Displayer dis_surface;

void dis_surface_init()
{
	DisplayerMake( &dis_surface );
	
	dis_surface.super.task = (Task)draw_triangle_mesh;
	
	/* read OBJ file */
	std::vector<SlVector3> obj_verts;
	std::vector<SlTri> obj_faces;
	
	bool success = readObjFile( "../obj/belch.obj", obj_verts, obj_faces );
	assert( success );
	assert( obj_faces[0][0] == 0 );
	int v_count = obj_verts.size();
	int f_count = obj_faces.size();
	
	/* make vertex position array */
	double *vertices = (double *)malloc( v_count * 3 * sizeof( double ) );
	for ( int i = 0; i < v_count; i++ )
	{
		vertices[3 * i] = obj_verts[i][0];
		vertices[3 * i + 1] = obj_verts[i][1];
		vertices[3 * i + 2] = obj_verts[i][2];
	}
	
	/* make vertex Displayers */
	Displayer *vertex_kids = (Displayer *)malloc( f_count * 3 * sizeof( Displayer ) );
	for ( int i = 0; i < f_count; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			DisplayerMake( vertex_kids + 3 * i + j );
			vertex_kids[3 * i + j].x = vertices + 3 * obj_faces[i][j];
		}
	}
	
	static double position[3] = { 0.0, 0.0, 0.0 };
	dis_surface.x = position;
	
	for ( int i = 3 * f_count - 1; i >=0; i-- )
		TtListAdd( &dis_surface.super.kids, (Tt *)(vertex_kids + i) );
	
	TtActivateChild( (Tt *)&dis_surface, v_count ); /* vertex count */
}

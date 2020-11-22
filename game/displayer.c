#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <string.h>
#include <vector>

#include "displayer.h"
#include "tt.h"
#include "vector.h"

/* the third element of the list */
static TtList *next_tri( TtList *t );

/* copy the first three list elements' position pointers into an array */
static void copy_tri( double **tri, TtList *t );

/* the normal vector of the plane defined by an array of three points */
static Vector tri_normal( double **tri );

/* compute and record vertex normal directions */
static void accumulate_normals( TtList *vertex_kids, double *vertices, double *normals );

/* send OpenGL commands for drawing a list of triangles */
static void draw_triangles( TtList *vertex_kids, double *vertices, double *normals );



/* constructor for Displayer */
void DisplayerMake( Displayer *d )
{	
	TtMake( (Tt *)d );
	
	d->x = NULL;
	
	return;
}



/* draw a sphere of radius 20 cm at the position of D */
Event draw_sphere( Displayer *d )
{
	static GLfloat sphere_color[4] = { 0.9f, 0.0f, 0.65f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphere_color);
	
	glPushMatrix();
	
	glTranslatef( d->x[0], d->x[1], d->x[2] );
	
	GLUquadric *q = gluNewQuadric();
	assert( q != 0 );
	gluSphere( q, 0.2, 13, 13 );
	gluDeleteQuadric( q );
	
	glPopMatrix();
	
	return NULL_EVENT;
}



/* draw a sphere of radius 1 m at the position of D */
Event draw_big_sphere( Displayer *d )
{
	static GLfloat color[4] = { 0.65f, 0.0f, 0.9f, 1.0f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
	
	glPushMatrix();
	
	glTranslatef( d->x[0], d->x[1], d->x[2] );
	
	GLUquadric *q = gluNewQuadric();
	assert( q != 0 );
	gluSphere( q, 1.0, 40, 40 );
	gluDeleteQuadric( q );
	
	glPopMatrix();
	
	return NULL_EVENT;
}



/* draw a quad with vertices at kids' positions, translated by D's position */
Event draw_quadrilateral( Displayer *d )
{
	Vector vertices[4];
	Vector normal;
	int j = 0;
	
	for ( TtList *i = ((Tt *)d)->kids; !TtListEmpty( i ); i = TtListRest( i ) )
	{
		assert( j < 4 ); /* use this func only on a tree with 4 children */
		vertices[j] = vec_make( ((Displayer *)TtListFirst( i ))->x );
		j++;
	}
	
	static GLfloat color[4] = { 0.5f, 0.05f, 0.15f, 1.0f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
	
	glPushMatrix();
	glTranslatef( d->x[0], d->x[1], d->x[2] );
	glBegin( GL_QUADS );
	
	for ( j = 0; j < 4; j++ )
	{
		normal =
			vec_normed(
				vec_cross(
					vec_diff(
						vertices[(j + 1) % 4],
						vertices[j % 4] ),
					vec_diff(
						vertices[(j - 1 + 4) % 4],
						vertices[j % 4] ) ) );
				
		glNormal3dv( normal.x );
		glVertex3dv( vertices[j].x );
	}
	
	glEnd();
	glPopMatrix();
	
	return NULL_EVENT;
}



/* draw triangles defined by triplets of kids, translated by D's position */
Event draw_triangle_mesh( Displayer *d )
{
	/* retrieve vertex data */
	unsigned int vertex_count = (unsigned int)TtListFirst( ((Tt *)d)->kids );
	TtList *vertex_kids = TtListRest( ((Tt *)d)->kids );
	double *vertices = ((Displayer *)TtListFirst( vertex_kids ))->x;
	
	/* prepare temporary storage for normals */
	double *normals = (double *)malloc( vertex_count * 3 * sizeof( double ) ); assert( normals != NULL );
	memset( normals, 0, vertex_count * 3 * sizeof( double ) );
	
	/* compute normals */
	accumulate_normals( vertex_kids, vertices, normals );
	
	for ( unsigned int i = 0; i < vertex_count; i++ ) {
		if ( vec_mag( vec_make( normals + 3 * i ) ) != 0 )
		{
			vec_copy(
				normals + 3 * i,
				vec_normed(	vec_make( normals + 3 * i ) ) );
		}
	}
	
	/* send commands for drawing mesh  */
	static GLfloat color[4] = { 0.0f, 0.3f, 0.05f, 1.0f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color );
	glPushMatrix();
	glTranslatef( d->x[0], d->x[1], d->x[2] );
	glBegin( GL_TRIANGLES );
	
	draw_triangles( vertex_kids, vertices, normals );
	
	glEnd();
	glPopMatrix();
	
	/* deallocate temporary storage for normals */
	free( normals );
	
	return NULL_EVENT;
}



TtList *next_tri( TtList *t )
{
	return TtListRest( TtListRest( TtListRest( t ) ) );
}



void copy_tri( double **tri, TtList *t )
{
	tri[0] = ((Displayer *)TtListFirst( t ))->x;
	tri[1] = ((Displayer *)TtListFirst( TtListRest( t ) ))->x;
	tri[2] = ((Displayer *)TtListFirst( TtListRest( TtListRest( t ) ) ))->x;
	
	return;
}



Vector tri_normal( double **tri )
{
	return
		vec_normed(
			vec_cross(
				vec_diff(
					vec_make( tri[1] ),
					vec_make( tri[0] ) ),
				vec_diff(
					vec_make( tri[2] ),
					vec_make( tri[0] ) ) ) );
}



void accumulate_normals( TtList *vertex_kids, double *vertices, double *normals )
{
	double *current_tri[3];
	int v_index = -1;
	Vector current_n;
	
	for ( TtList *i = vertex_kids; !TtListEmpty( i ); i = next_tri( i ) )
	{
		copy_tri( current_tri, i );
		current_n = tri_normal( current_tri );

		for ( int j = 0; j < 3; j++ )
		{
			v_index = (current_tri[j] - vertices) / 3;
			
			vec_copy(
				normals + 3 * v_index,
				vec_sum(
					vec_make( normals + 3 * v_index ),
					current_n ) );
		}
	}
	
	return;
}



void draw_triangles( TtList *vertex_kids, double *vertices, double *normals )
{
	double *current_tri[3];
	int v_index = -1;
	
	for ( TtList *i = vertex_kids; !TtListEmpty( i ); i = next_tri( i ) )
	{
		copy_tri( current_tri, i );
		
		for ( int j = 0; j < 3; j++ )
		{
			v_index = (current_tri[j] - vertices) / 3;
			
			glNormal3dv( normals + 3 * v_index );
			glVertex3dv( vertices + 3 * v_index );
		}
	}
	
	return;
}

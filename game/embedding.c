#include <slMatrix.H>
#include <slVector.H>
#include <string.h>

#include "barymap.h"
#include "displayer.h"
#include "tt.h"
#include "vector.h"

/* copy the first three list elements' position pointers into an array */
static void copy_tet( double **tri, TtList *t );

/* the list of barymaps in a tet's list of kids */
static TtList *barymaps( TtList *t );

Event embed_surface_in_tetmesh( Tt *embedding )
{
	double *current_tet[4];
	SlMatrix3x3 B;
	for ( TtList *i = embedding->kids; !TtListEmpty( i ); i = TtListRest( i ) )
	{
		/* first 4 kids are tet vertices, rest are surface vertices inside tet */
		TtList *kids = TtListFirst( i )->kids;
		
		copy_tet( current_tet, kids );
		B = SlMatrix3x3(
				SlVector3( current_tet[0] ) - SlVector3( current_tet[3] ),
				SlVector3( current_tet[1] ) - SlVector3( current_tet[3] ),
				SlVector3( current_tet[2] ) - SlVector3( current_tet[3] ),
				SlMatrix3x3::col );
		
		Barymap *v;
		for ( TtList *j = barymaps( kids ); !TtListEmpty( j ); j = TtListRest( j ) )
		{
			v = (Barymap *)TtListFirst( j );
			//memset( v->x, 0, 3 * sizeof( double ) );
			vec_copy(
				v->x,
				vec_make(
					(SlVector3( current_tet[3] ) + B * SlVector3( v->b )).data ) );
		}
	}
	
	return NULL_EVENT;
}



void copy_tet( double **tet, TtList *t )
{
	tet[0] = ((Displayer *)TtListFirst( t ))->x;
	tet[1] = ((Displayer *)TtListFirst( TtListRest( t ) ))->x;
	tet[2] = ((Displayer *)TtListFirst( TtListRest( TtListRest( t ) ) ))->x;
	tet[3] = ((Displayer *)TtListFirst( TtListRest( TtListRest( TtListRest( t ) ) ) ))->x;
	
	return;
}



TtList *barymaps( TtList *t )
{
	return TtListRest( TtListRest( TtListRest( TtListRest( t ) ) ) );
}

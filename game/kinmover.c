#include <math.h>
#include <stdio.h>
#include <string.h>

#include "kinmover.h"
#include "tt.h"
#include "vector.h"

extern double g_dt;
extern double g_grav_acc[3];
extern double g_pi;

extern char g_key[];
extern double g_mouseDragX, g_mouseDragY;



void KinMoverMake( KinMover *k )
{
	TtMake( (Tt *)k );
	
	memset( k->x, 0, 3 * sizeof( double ) );
	memset( k->v, 0, 3 * sizeof( double ) );
	memset( k->rotations, 0, 3 * sizeof( double ) );
}



/* set position as if moving at previous velocity over whole step */ 
Event inertial_move( KinMover *k )
{
	vec_copy(
		k->x,
		vec_sum(
			vec_make( k->x ),
			vec_scaled( vec_make( k->v ), g_dt ) ) );
	
	return NULL_EVENT;
}



/* Euler-integrate position and velocity with acceleration due to gravity */
Event gravity_move( KinMover *k )
{
	vec_copy(
		k->v,
		vec_sum(
			vec_make( k->v ),
			vec_scaled( vec_make( g_grav_acc ), g_dt ) ) );
	
	inertial_move( k );
	
	return NULL_EVENT;
}



/* set velocity based on user input */
Event input_move( KinMover *k )
{
	Vector out, right;
	
	k->rotations[0] = fmod( (k->rotations[0] + -g_mouseDragY * g_pi ) + 2 * g_pi, 2 * g_pi );
	k->rotations[1] = fmod( (k->rotations[1] + -g_mouseDragX * g_pi ) + 2 * g_pi, 2 * g_pi );
	
	out.x[0] = sin( -k->rotations[1] );
	out.x[1] = 0;
	out.x[2] = -cos( -k->rotations[1] );
	
	right.x[0] = sin( -(k->rotations[1] - g_pi / 2.0) );
	right.x[1] = 0;
	right.x[2] = -cos( -(k->rotations[1] - g_pi / 2.0) );
	
	//fprintf( stderr, "out: [ %f, %f, %f ]\n", out.x[0], out.x[1], out.x[2] );
	
	memset( k->v, 0, 3 * sizeof( double ) );
	
	if ( g_key['w'] ) {
		vec_copy(
			k->v,
			vec_sum(
				vec_make( k->v ),
				vec_scaled( out, 4.0 ) ) );
	}
	else if ( g_key['s'] ) {
		vec_copy(
			k->v,
			vec_sum(
				vec_make( k->v ),
				vec_scaled( out, -4.0 ) ) );
	}
	
	if ( g_key['d'] ) {
		vec_copy(
			k->v,
			vec_sum(
				vec_make( k->v ),
				vec_scaled( right, 4.0 ) ) );
	}
	else if ( g_key['a'] ) {
		vec_copy(
			k->v,
			vec_sum(
				vec_make( k->v ),
				vec_scaled( right, -4.0 ) ) );
	}
	
	inertial_move( k );
	
	return NULL_EVENT;
}

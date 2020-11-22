#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "collider.h"
#include "damager.h"
#include "displayer.h"
#include "fire.h"
#include "kinmover.h"
#include "projtracker.h"
#include "timer.h"
#include "tt.h"
#include "vector.h"

static void init_projectile_projtracker( Fire *f );
static void init_projectile_collider( Fire *f );
static void init_projectile_displayer( Fire *f );
static void init_projectile_kinmover( Fire *f );
static void init_projectile_damager( Fire *f );

extern double g_pi;
extern char g_mouseHeld;



void FireMake( Fire *f )
{
	TtMake( (Tt *)f );
	
	f->x = NULL;
	f->rotations = NULL;
	
	f->col = NULL;
	f->dis = NULL;
	f->kin = NULL;
	
	return;
}



Event launch_projectile( Fire *fire )
{
	Event e = EventMake();
	
	if ( g_mouseHeld )
	{
		fire->tra = (ProjTracker *)malloc( sizeof( ProjTracker ) );
		ProjTrackerMake( fire->tra );
		
		fire->col = (Collider *)malloc( sizeof( Collider ) );
		ColliderMake( fire->col );
		
		fire->dis = (Displayer *)malloc( sizeof( Displayer ) );
		DisplayerMake( fire->dis );
		
		fire->kin = (KinMover *)malloc( sizeof( KinMover ) );
		KinMoverMake( fire->kin );
		
		fire->dam = (Tt *)malloc( sizeof( Tt ) );
		TtMake( fire->dam );
		
		init_projectile_projtracker( fire );
		init_projectile_collider( fire );
		init_projectile_displayer( fire );
		init_projectile_kinmover( fire );
		init_projectile_damager( fire );
		
		SigListAdd( &e.sigs, FIRE_LAUNCH );
	}
	
	return e;
}



void init_projectile_projtracker( Fire *f )
{
	((Tt *)f->tra)->task = (Task)track_projectile;
	
	f->tra->obstacle = new SphereObstacle( SlVector3( 0.0 ), SlVector3( 0.0 ), 0.4 );
	f->tra->k = f->kin;
}



void init_projectile_collider( Fire *f )
{
	((Tt *)f->col)->task = identify_projectile_collider;
	
	Tt *response = (Tt *)malloc( sizeof( Tt ) );
	TtMake( response );
	response->task = (Task)elastic_response;
	
	f->col->x = f->kin->x;
	f->col->v = f->kin->v;
	
	f->col->m = 10.0;
	
	f->col->min[0] = -0.2;
	f->col->min[1] = -0.2;
	f->col->min[2] = -0.2;
	f->col->max[0] = 0.2;
	f->col->max[1] = 0.2;
	f->col->max[2] = 0.2;
	
	TtActivateChild( (Tt *)f->col, response );
}



void init_projectile_displayer( Fire *f )
{
	((Tt *)f->dis)->task = (Task)draw_sphere;
		
	f->dis->x = f->kin->x;
}



void init_projectile_kinmover( Fire *f )
{
	Vector aim, right;
	
	((Tt *)f->kin)->task = (Task)gravity_move;
	
	right.x[0] = sin( -(f->rotations[1] - g_pi / 2.0) );
	right.x[1] = sin( f->rotations[0] );
	right.x[2] = -cos( -(f->rotations[1] - g_pi / 2.0) );
	
	aim.x[0] = sin( -f->rotations[1] );
	aim.x[1] = sin( f->rotations[0] );
	aim.x[2] = -cos( -f->rotations[1] );
	
	vec_copy(
		f->kin->x,
		vec_sum(
			vec_make( f->x ),
			vec_sum(
				vec_scaled( aim, 0.65 ),
				vec_scaled( right, 0.65 ) ) ) );
	
	vec_copy( f->kin->v, vec_scaled( aim, 10.0 ) );
}



void init_projectile_damager( Fire *f )
{
	((Tt *)f->dam)->task = (Task)damage_projectile;
	
	Timer *timer = (Timer *)malloc( sizeof( Timer ) );
	TimerMake( timer );
	((Tt *)timer)->task = (Task)count_down;
	timer->t = 2.0;
	
	TtActivateChild( f->dam, (Tt *)f->tra );
	TtActivateChild( f->dam, (Tt *)f->col );
	TtActivateChild( f->dam, (Tt *)f->dis );
	TtActivateChild( f->dam, (Tt *)f->kin );
	TtActivateChild( f->dam, (Tt *)timer );
}

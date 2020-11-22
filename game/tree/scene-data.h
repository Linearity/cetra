#include "kinematics-data.h"
#include "collisions-data.h"
#include "camera-data.h"
#include "fire-data.h"
#include "fire-timer-data.h"
#include "health-data.h"
#include "deformation-data.h"

Tt scene;

Event update_scene( Tt *scene ) /* NOTE: scene is a pointer in this function */
{
	Event e = EventMake(), f = EventMake();
	
	for ( TtList *i = scene->kids; !TtListEmpty( i ); i = TtListRest( i ) )
		EventInclude( &e, TtExec( TtListFirst( i ) ) );
	
	Tt *projectile;
	for ( SigList *i = e.sigs; !SigListEmpty( i ); i = SigListRest( i ) ) {
		switch ( SigListFirst( i ) )
		{
		case FIRE_LAUNCH:
			TtActivateChild( (Tt *)&deformation, (Tt *)fire.tra );
			TtActivateChild( (Tt *)&kinematics, (Tt *)fire.kin );
			TtActivateChild( (Tt *)&collisions, (Tt *)fire.col );
			TtActivateChild( (Tt *)&camera, (Tt *)fire.dis );
			TtActivateChild( (Tt *)&health, (Tt *)fire.dam );
			
			TtDeactivateChild( scene, (Tt *)&fire );
			
			fire_timer.t = 0.25;
			TtActivateChild( scene, (Tt *)&fire_timer );
			
			break;
			
		case TIMER_ZERO:
			TtDeactivateChild( scene, (Tt *)&fire_timer );
			TtActivateChild( scene, (Tt *)&fire );
			
			break;
		
		case HEALTH_PROJ_DESTROYED:
			projectile = TtListFirst( health.destroyed );
			
			TtDeactivateChild( &kinematics, TtListFirst( projectile->kids ) );
			TtListRemove( &projectile->kids, TtListFirst( projectile->kids ) );
			
			TtDeactivateChild( (Tt *)&camera, TtListFirst( projectile->kids ) );
			TtListRemove( &projectile->kids, TtListFirst( projectile->kids ) );
			
			TtDeactivateChild( (Tt *)&collisions, TtListFirst( projectile->kids ) );
			TtListRemove( &projectile->kids, TtListFirst( projectile->kids ) );
			
			TtDeactivateChild( (Tt *)&deformation, TtListFirst( projectile->kids ) );
			TtListRemove( &projectile->kids, TtListFirst( projectile->kids ) );
			
			TtListRemove( &health.destroyed, projectile );
			
			break;
			
		case COLLISIONS_PROJ_TARGET:
			break;
		}
	}
	
	return f;
}

void scene_init()
{
	TtMake( &scene );
	
	scene.task = update_scene;
	
	camera_init();
	collisions_init();
	kinematics_init();
	fire_init();
	fire_timer_init();
	health_init();
	deformation_init();
	
	TtActivateChild( &scene, (Tt *)&health );
	//TtActivateChild( &scene, (Tt *)&camera );
	TtActivateChild( &scene, (Tt *)&collisions );
	TtActivateChild( &scene, (Tt *)&deformation );
	TtActivateChild( &scene, (Tt *)&kinematics );
	TtActivateChild( &scene, (Tt *)&fire );
}

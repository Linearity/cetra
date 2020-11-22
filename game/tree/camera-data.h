/* camera */

#include "dis-ball-data.h"
//#include "dis-ball2-data.h"
#include "dis-wall1-data.h"
#include "dis-wall2-data.h"
#include "dis-wall3-data.h"
#include "dis-wall4-data.h"
#include "dis-floor-data.h"
#include "dis-target-data.h"

Camera camera;

void camera_init()
{
	CameraMake( &camera );
	
	camera.super.task = (Task)&render;
	
	dis_ball_init();
	//dis_ball2_init();
	dis_wall1_init();
	dis_wall2_init();
	dis_wall3_init();
	dis_wall4_init();
	dis_floor_init();
	dis_target_init();
	//dis_target_bb_init();
	
	/* pointer to kinematic data */
	camera.x = kin_gun.x;
	camera.rotations = kin_gun.rotations;
	
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_target );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_floor );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall1 );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall2 );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall3 );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall4 );
	TtActivateChild( (Tt *)&camera, (Tt *)&dis_ball );
	//TtActivateChild( (Tt *)&camera, (Tt *)&dis_ball2 );
	//TtActivateChild( (Tt *)&camera, (Tt *)&dis_target_bb );
}

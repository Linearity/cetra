/* kinematics */

#include "kin-gun-data.h"
#include "kin-ball-data.h"
//#include "kin-ball2-data.h"
#include "kin-wall1-data.h"
#include "kin-wall2-data.h"
#include "kin-wall3-data.h"
#include "kin-wall4-data.h"
#include "kin-floor-data.h"

Tt kinematics;

void kinematics_init()
{
	TtMake( &kinematics );
	
	kinematics.task = integrate_kinematics;
	
	kin_gun_init();
	kin_ball_init();
	//kin_ball2_init();
	kin_wall1_init();
	kin_wall2_init();
	kin_wall3_init();
	kin_wall4_init();
	kin_floor_init();
	
	TtActivateChild( &kinematics, (Tt *)&kin_gun );
	TtActivateChild( &kinematics, (Tt *)&kin_ball );
	//TtActivateChild( &kinematics, (Tt *)&kin_ball2 );
	TtActivateChild( &kinematics, (Tt *)&kin_wall1 );
	TtActivateChild( &kinematics, (Tt *)&kin_wall2 );
	TtActivateChild( &kinematics, (Tt *)&kin_wall3 );
	TtActivateChild( &kinematics, (Tt *)&kin_wall4 );
	TtActivateChild( &kinematics, (Tt *)&kin_floor );
}

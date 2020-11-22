/* kinematics */

Tt kinematics;
TtMake( &kinematics );

kinematics.task = integrate_kinematics;

#include "kin-gun.c"
#include "kin-ball.c"
#include "kin-ball2.c"
#include "kin-wall1.c"
#include "kin-wall2.c"
#include "kin-floor.c"

TtActivateChild( &kinematics, (Tt *)&kin_gun );
TtActivateChild( &kinematics, (Tt *)&kin_ball );
TtActivateChild( &kinematics, (Tt *)&kin_ball2 );
TtActivateChild( &kinematics, (Tt *)&kin_wall1 );
TtActivateChild( &kinematics, (Tt *)&kin_wall2 );
TtActivateChild( &kinematics, (Tt *)&kin_floor );

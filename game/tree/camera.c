/* camera */

Camera camera;
CameraMake( &camera );

camera.super.task = (Task)&render;

/* pointer to kinematic data */
camera.x = kin_gun.x;
camera.rotations = kin_gun.rotations;

#include "dis-ball.c"
#include "dis-ball2.c"
#include "dis-wall1.c"
#include "dis-wall2.c"
#include "dis-floor.c"

TtActivateChild( (Tt *)&camera, (Tt *)&dis_floor );
TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall1 );
TtActivateChild( (Tt *)&camera, (Tt *)&dis_wall2 );
TtActivateChild( (Tt *)&camera, (Tt *)&dis_ball );
TtActivateChild( (Tt *)&camera, (Tt *)&dis_ball2 );

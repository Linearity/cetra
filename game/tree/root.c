/* root */

Tt root;
TtMake( &root );

root.task = update_game;

#include "kinematics.c"
#include "camera.c"
#include "collisions.c"

TtActivateChild( &root, (Tt *)&camera );
TtActivateChild( &root, (Tt *)&collisions );
TtActivateChild( &root, (Tt *)&kinematics );

/* collisions */

Tt collisions;
TtMake( &collisions );

collisions.task = resolve_collisions;

#include "col-gun.c"
#include "col-ball.c"
#include "col-ball2.c"
#include "col-wall1.c"
#include "col-wall2.c"
#include "col-floor.c"

TtActivateChild( (Tt *)&collisions, (Tt *)&col_gun );
TtActivateChild( (Tt *)&collisions, (Tt *)&col_ball );
TtActivateChild( (Tt *)&collisions, (Tt *)&col_ball2 );
TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall1 );
TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall2 );
TtActivateChild( (Tt *)&collisions, (Tt *)&col_floor );

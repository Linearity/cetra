/* collisions */

#include "col-gun-data.h"
#include "col-ball-data.h"
//#include "col-ball2-data.h"
#include "col-wall1-data.h"
#include "col-wall2-data.h"
#include "col-wall3-data.h"
#include "col-wall4-data.h"
#include "col-floor-data.h"

Tt collisions;

void collisions_init()
{
	TtMake( &collisions );
	
	collisions.task = resolve_collisions;
	
	col_gun_init();
	col_ball_init();
	//col_ball2_init();
	col_wall1_init();
	col_wall2_init();
	col_wall3_init();
	col_wall4_init();
	col_floor_init();
	
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_gun );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_ball );
	//TtActivateChild( (Tt *)&collisions, (Tt *)&col_ball2 );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall1 );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall2 );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall3 );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_wall4 );
	TtActivateChild( (Tt *)&collisions, (Tt *)&col_floor );
}

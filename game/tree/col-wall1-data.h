/* wall1 collision */

Collider col_wall1;
Tt col_wall1_response;

void col_wall1_init()
{
	ColliderMake( &col_wall1 );
	
	col_wall1.super.task = identify_wall_collider;
	
	TtMake( &col_wall1_response );
	col_wall1_response.task = NULL_TASK;
	
	/* mass in kilograms */
	col_wall1.m = 1.0e9;
	
	/* bounding box corners in meters */
	col_wall1.min[0] = -7.0;
	col_wall1.min[1] = -7.0;
	col_wall1.min[2] = -5.0;
	
	col_wall1.max[0] = 7.0;
	col_wall1.max[1] = 7.0;
	col_wall1.max[2] = 0.0;
	
	/* pointers to kinematic data */
	col_wall1.x = kin_wall1.x;
	col_wall1.v = kin_wall1.v;

	TtActivateChild( (Tt *)&col_wall1, (Tt *)&col_wall1_response );
}

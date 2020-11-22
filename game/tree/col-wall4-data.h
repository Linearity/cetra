/* wall4 collision */

Collider col_wall4;
Tt col_wall4_response;

void col_wall4_init()
{
	ColliderMake( &col_wall4 );
	
	col_wall4.super.task = identify_wall_collider;
	
	TtMake( &col_wall4_response );
	col_wall4_response.task = NULL_TASK;
	
	/* mass in kilograms */
	col_wall4.m = 1.0e9;
	
	/* bounding box corners in meters */
	col_wall4.min[0] = -7.0;
	col_wall4.min[1] = -7.0;
	col_wall4.min[2] = 0.0;
	
	col_wall4.max[0] = 7.0;
	col_wall4.max[1] = 7.0;
	col_wall4.max[2] = 5.0;
	
	/* pointers to kinematic data */
	col_wall4.x = kin_wall4.x;
	col_wall4.v = kin_wall4.v;
	
	TtActivateChild( (Tt *)&col_wall4, (Tt *)&col_wall4_response );
}

/* wall3 collision */

Collider col_wall3;
Tt col_wall3_response;

void col_wall3_init()
{
	ColliderMake( &col_wall3 );
	
	col_wall3.super.task = identify_wall_collider;
	
	TtMake( &col_wall3_response );
	col_wall3_response.task = NULL_TASK;
	
	/* mass in kilograms */
	col_wall3.m = 1.0e9;
	
	/* bounding box corners in meters */
	col_wall3.min[0] = 0.0;
	col_wall3.min[1] = -7.0;
	col_wall3.min[2] = -7.0;
	
	col_wall3.max[0] = 5.0;
	col_wall3.max[1] = 7.0;
	col_wall3.max[2] = 10.0;
	
	/* pointers to kinematic data */
	col_wall3.x = kin_wall3.x;
	col_wall3.v = kin_wall3.v;

	TtActivateChild( (Tt *)&col_wall3, (Tt *)&col_wall3_response );
}

/* wall2 collision */

Collider col_wall2;
Tt col_wall2_response;

void col_wall2_init()
{
	ColliderMake( &col_wall2 );
	
	col_wall2.super.task = identify_wall_collider;
	
	TtMake( &col_wall2_response );
	col_wall2_response.task = NULL_TASK;
	
	/* mass in kilograms */
	col_wall2.m = 1.0e9;
	
	/* bounding box corners in meters */
	col_wall2.min[0] = -5.0;
	col_wall2.min[1] = -7.0;
	col_wall2.min[2] = -7.0;
	
	col_wall2.max[0] = 0.0;
	col_wall2.max[1] = 7.0;
	col_wall2.max[2] = 10.0;
	
	/* pointers to kinematic data */
	col_wall2.x = kin_wall2.x;
	col_wall2.v = kin_wall2.v;
	
	TtActivateChild( (Tt *)&col_wall2, (Tt *)&col_wall2_response );
}

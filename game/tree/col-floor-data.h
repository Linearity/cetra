/* floor collision */

Collider col_floor;
Tt col_floor_response;

void col_floor_init()
{
	ColliderMake( &col_floor );
	
	col_floor.super.task = identify_floor_collider;
	
	TtMake( &col_floor_response );
	col_floor_response.task = NULL_TASK;
	
	/* mass in kilograms */
	col_floor.m = 1.0e9;
	
	/* bounding box corners in meters */
	col_floor.min[0] = -7.0;
	col_floor.min[1] = -5.0;
	col_floor.min[2] = -7.0;
	
	col_floor.max[0] = 7.0;
	col_floor.max[1] = 0.0;
	col_floor.max[2] = 10.0;
	
	/* pointers to kinematic data */
	col_floor.x = kin_floor.x;
	col_floor.v = kin_floor.v;
	
	TtActivateChild( (Tt *)&col_floor, (Tt *)&col_floor_response );
}

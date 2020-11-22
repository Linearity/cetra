/* ball collision */

Collider col_ball2;
Tt col_ball2_response;

void col_ball2_init()
{
	ColliderMake( &col_ball2 );
	
	col_ball2.super.task = identify_ball_collider;
	
	TtMake( &col_ball2_response );
	col_ball2_response.task = (Task)elastic_response;
	
	/* mass in kilograms */
	col_ball2.m = 500.0;
	
	/* bounding box corners in meters */
	col_ball2.min[0] = -0.8;
	col_ball2.min[1] = -0.8;
	col_ball2.min[2] = -0.8;
	
	col_ball2.max[0] = 0.8;
	col_ball2.max[1] = 0.8;
	col_ball2.max[2] = 0.8;
	
	/* pointers to kinematic data */
	col_ball2.x = kin_ball2.x;
	col_ball2.v = kin_ball2.v;
	
	TtActivateChild( (Tt *)&col_ball2, (Tt *)&col_ball2_response );
}

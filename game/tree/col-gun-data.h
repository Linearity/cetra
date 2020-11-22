/* gun collision */

Collider col_gun;
Tt col_gun_response;

void col_gun_init()
{
	ColliderMake( &col_gun );
	
	col_gun.super.task = identify_gun_collider;
	
	TtMake( &col_gun_response );
	col_gun_response.task = (Task)plastic_response;
	
	/* mass in kilograms */
	col_gun.m = 80.0;
	
	/* bounding box corners in meters */
	col_gun.min[0] = -0.25;
	col_gun.min[1] = -0.7;
	col_gun.min[2] = -0.25;
	
	col_gun.max[0] = 0.25;
	col_gun.max[1] = 0.7;
	col_gun.max[2] = 0.25;
	
	/* pointers to kinematic data */
	col_gun.x = kin_gun.x;
	col_gun.v = kin_gun.v;
	
	TtActivateChild( (Tt *)&col_gun, (Tt *)&col_gun_response );
}

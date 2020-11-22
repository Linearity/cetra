/* target collider */

Collider col_target;

void col_target_init()
{
	ColliderMake( &col_target );
	
	col_target.super.task = identify_target_collider;
	
	col_target.resoluble = 0;
}

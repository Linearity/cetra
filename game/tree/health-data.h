/* health */

Health health;

void health_init()
{
	HealthMake( &health );
	
	health.super.task = (Task)update_health;
}

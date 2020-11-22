/* fire timer */

Timer fire_timer;

void fire_timer_init()
{
	TimerMake( &fire_timer );
	
	fire_timer.super.task = (Task)count_down;
}

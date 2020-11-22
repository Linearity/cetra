#include <stdio.h>
#include "timer.h"
#include "tt.h"

extern double g_dt;



void TimerMake( Timer *timer )
{
	TtMake( (Tt *)timer );
	timer->t = 0.0;
}



Event count_down( Timer *timer )
{
	Event e = EventMake();
	
	timer->t -= g_dt;
	
	if ( timer->t <= 0.0 )
	{
		timer->t = 0.0;
		SigListAdd( &e.sigs, TIMER_ZERO );
	}
	
	return e;
}

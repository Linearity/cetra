#ifndef __TIMER_H__
#define __TIMER_H__

#include "tt.h"

typedef struct Timer Timer;

struct Timer
{
	Tt super;

	double t;
};

void TimerMake( Timer *timer );

Event count_down( Timer *timer );

#endif

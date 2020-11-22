#ifndef __HEALTH_H__
#define __HEALTH_H__

#include "tt.h"

typedef struct Health Health;

struct Health
{
	Tt super;
	
	TtList *destroyed;
};

void HealthMake( Health *h );

Event update_health( Health *health );

#endif

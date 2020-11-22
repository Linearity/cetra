#ifndef __EVENT_H__
#define __EVENT_H__

typedef short Signal;
typedef struct LinkedList SigList;

SigList *SigListMake();
Signal SigListFirst( SigList *list );
SigList *SigListRest( SigList *list );
int SigListEmpty( SigList *list );
void SigListAdd( SigList **list_ptr, Signal s );
void SigListRemove( SigList **list_ptr, Signal s );

typedef struct Event Event;

struct Event	/* Event base class */
{
	SigList *sigs;
};

Event EventMake();
void EventInclude( Event *including, Event included );

extern Event g_null_event;
#define NULL_EVENT g_null_event

enum
{
	FIRE_LAUNCH,
	TIMER_ZERO,
	HEALTH_PROJ_DESTROYED,
	DAMAGER_PROJ,
	COLLIDER_PROJ,
	COLLIDER_TARGET,
	COLLIDER_GUN,
	COLLIDER_FLOOR,
	COLLIDER_WALL,
	COLLIDER_BALL,
	COLLISIONS_PROJ_PROJ,
	COLLISIONS_PROJ_BALL,
	COLLISIONS_PROJ_TARGET,
	COLLISIONS_PROJ_GUN,
	PROJTRACKER_TRACK
};

#endif

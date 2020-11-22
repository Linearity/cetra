#include "kinmover.h"
#include "projtracker.h"

void ProjTrackerMake( ProjTracker *p )
{
	TtMake( (Tt *)p );
	
	p->k = NULL;
	p->obstacle = NULL;
}

Event track_projectile( ProjTracker *p )
{
	Event e = EventMake();
	
	p->obstacle->center[0] = p->k->x[0];
	p->obstacle->center[1] = -p->k->x[2];
	p->obstacle->center[2] = p->k->x[1];
	
	p->obstacle->vel[0] = p->k->v[0];
	p->obstacle->vel[1] = -p->k->v[2];
	p->obstacle->vel[2] = p->k->v[1];
	
	SigListAdd( &e.sigs, PROJTRACKER_TRACK );
	return e;
}

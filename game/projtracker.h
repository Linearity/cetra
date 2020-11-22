#ifndef __PROJTRACKER_H__
#define __PROJTRACKER_H__

#include <obstacle.H>
#include "kinmover.h"

struct ProjTracker
{
	Tt super;
	
	KinMover *k;
	SphereObstacle *obstacle;
};

void ProjTrackerMake( ProjTracker *p );

Event track_projectile( ProjTracker *p );

#endif

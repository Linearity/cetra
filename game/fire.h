#ifndef __FIRE_H__
#define __FIRE_H__

#include "collider.h"
#include "displayer.h"
#include "kinmover.h"
#include "projtracker.h"
#include "tt.h"

typedef struct Fire Fire;

struct Fire
{
	Tt super;
	
	double *x;
	double *rotations;
	ProjTracker *tra;
	Collider *col;
	Displayer *dis;
	KinMover *kin;
	Tt *dam;
};

void FireMake( Fire *f );

Event launch_projectile( Fire *fire );

#endif

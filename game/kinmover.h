#ifndef __KINMOVER_H__
#define __KINMOVER_H__

#include "tt.h"

typedef struct KinMover KinMover;

struct KinMover
{
	Tt super;
	
	double x[3];
	double v[3];
	double rotations[3];
};

void KinMoverMake( KinMover *k );

Event inertial_move( KinMover * );
Event gravity_move( KinMover * );
Event input_move( KinMover * );

#endif

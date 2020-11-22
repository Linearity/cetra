#ifndef __DEFORMATION_H__
#define __DEFORMATION_H__

#include <fem.H>

#include "tt.h"

typedef struct Deformation Deformation;

struct Deformation
{
	Tt super;
	
	FemSimulator *simulator;
};

void DeformationMake( Deformation *d );

Event step_simulation( Deformation *deformation );

#endif

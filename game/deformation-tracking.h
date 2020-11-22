#ifndef __DEFORMATION_H__
#define __DEFORMATION_H__

#include <fem.H>

#include "tt.h"

typedef struct Deformation Deformation;

struct Deformation
{
	Tt super;
	
	FemSimulator *simulator;
	TtList *proj_enter;
	TtList *proj_exit;
};

void DeformationMake( Deformation *d );

Event step_simulation( Deformation *deformation );

#endif

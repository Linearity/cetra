#ifndef __DEFORMER_H__
#define __DEFORMER_H__

#include <fem.H>
#include "collider.h"
#include "tt.h"

struct Deformer
{
	Tt super;
	
	double *vertices;
	FemObject *object;
};

void DeformerMake( Deformer *d );

Event deform( Deformer *d );

#endif

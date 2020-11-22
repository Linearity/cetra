#ifndef __BARYMAP_H__
#define __BARYMAP_H__

#include "tt.h"

typedef struct Barymap Barymap;

struct Barymap
{
	Tt super;
	
	double b[3];
	double *x;
};

void BarymapMake( Barymap *d );

#endif

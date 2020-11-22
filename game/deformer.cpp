#include <fem.H>     
#include <float.h>
#include <slVector.H>
#include <string.h>

#include "deformer.h"
#include "vector.h"



Event deform( Deformer *d )
{
	FemObject *object = d->object;
	
	/* set deformer vertices by transforming object's vertices */
	for ( unsigned int i = 0; i < object->nv; i++ )
	{
		(d->vertices + 3 * i)[0] = object->pos[i][0];
		(d->vertices + 3 * i)[1] = object->pos[i][2];
		(d->vertices + 3 * i)[2] = -object->pos[i][1];
	}
	
	return NULL_EVENT;
}



void DeformerMake( Deformer *d )
{
	TtMake( (Tt *)d );
	
	d->vertices = NULL;
	d->object = NULL;
}

#include <stdio.h>
#include <string.h>
#include "barymap.h"

/* constructor for Barymap */
void BarymapMake( Barymap *b )
{	
	TtMake( (Tt *)b );
	
	b->x = NULL;
	memset( b->b, 0, 3 * sizeof( double ) );
	
	return;
}

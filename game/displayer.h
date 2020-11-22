#ifndef __DISPLAYER_H__
#define __DISPLAYER_H__

#include "tt.h"

typedef struct Displayer Displayer;

struct Displayer
{
	Tt super;
	
	double *x;
};

void DisplayerMake( Displayer *d );

Event draw_sphere( Displayer *d );
Event draw_big_sphere( Displayer *d );
Event draw_quadrilateral( Displayer *d );
Event draw_triangle_mesh( Displayer *d );

#endif

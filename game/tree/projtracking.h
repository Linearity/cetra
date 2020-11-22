#ifndef __PROJTRACKING_H__
#define __PROJTRACKING_H__

#include "tt.h"

typedef struct ProjTracking ProjTracking;

struct ProjTracking
{
	Tt super;
	
	TtList *proj_enter;
};

Event manage_available_trackers( ProjTracking *p );

#endif

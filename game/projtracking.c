#include "projtracker.h"
#include "projtracking.h"

Event manage_available_trackers( ProjTracking *p )
{
	while trackers available:
	
	get first tracker "e" from enter list
	copy "e" tracker data to first tracker from kids list 
	
	if ( TtListEmpty( ((Tt *)target)->kids ) )
		SigListAdd( &e.sigs, PROJTRACKING_EMPTIED );
	else
		SigListAdd( &e.sigs, PROJTRACKING_NEW );
}

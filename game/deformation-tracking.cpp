#include <fem.H>
#include "deformation.h"
#include "tt.h"

extern double g_dt;

void DeformationMake( Deformation *d )
{
	TtMake( (Tt *)d );
	
	d->simulator = NULL;
}

Event step_simulation( Deformation *deformation )
{
	Event e = EventMake();
	
	deformation->simulator->dt = g_dt;
	deformation->simulator->step();
	
	TtListDestroy( proj_exit );
	proj_exit = TtListMake();
	
	for ( TtList *i = deformation->super.kids; !TtListEmpty( i ); i = TtListRest( i ) )
	{
		e = TtExec( TtListFirst( i ) );
		switch ( TtListFirst( e.sigs ) )
		{
		case PROJTRACKER_TRACK:
			ProjTracker *tracker = (ProjTracker *)TtListFirst( i );
			if ( !within( tracker, target ) )
			{
				set kinmover position to that of obstacle
				send obstacle home
				put tracker on exit list
				deactivate tracker here
				activate tracker on ProjTracking
				activate ProjTracking
			}
			break;
		case PROJTRACKING_NEW:
			activate TtListFirst( ProjTracking->kids ) here
			deactivate the same on ProjTracking
			
			break;
		case PROJTRACKING_EMPTIED:
			deactivate ProjTracking
			break;
	}
	
	TtListDestroy( proj_enter );
	proj_enter = TtListMake();
	
	return e;
}






#include <fem.H>
#include "deformation.h"
#include "projtracker.h"
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
	
	deformation->simulator->obstacles.clear();
	
	for ( TtList *i = deformation->super.kids; !TtListEmpty( i ); i = TtListRest( i ) )
	{
		e = TtExec( TtListFirst( i ) );
		
		switch ( SigListFirst( e.sigs ) )
		{
		case PROJTRACKER_TRACK:
			deformation->simulator->obstacles.push_back(
				((ProjTracker *)TtListFirst( i ))->obstacle );
			break;
		default:
			break;
		}
	}
	
	deformation->simulator->dt = g_dt;
	deformation->simulator->step();
	
	return NULL_EVENT;
}

#include <stdio.h>
#include "health.h"
#include "tt.h"



void HealthMake( Health *h )
{
	TtMake( (Tt *)h );
	h->destroyed = TtListMake();
}



Event update_health( Health *health )
{
	Event e = EventMake(), f = EventMake();
	
	Tt *first;
	for ( TtList *i = ((Tt *)health)->kids; !TtListEmpty( i ); i = TtListRest( i ) ) {
		first = TtListFirst( i );
		e = TtExec( first );
		for ( SigList *j = e.sigs; !SigListEmpty( j ); j = SigListRest( j ) ) {
			switch ( SigListFirst( j ) )
			{
			case DAMAGER_PROJ:
				TtDeactivateChild( (Tt *)health, first );
				
				TtListAdd( &health->destroyed, first );
				SigListAdd( &f.sigs, HEALTH_PROJ_DESTROYED );
				
				break;
			}
		}
	}
	
	return f;
}

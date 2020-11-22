#include <malloc.h>
#include <stdio.h>

#include "damager.h"
#include "tt.h"



Event damage_projectile( Tt *p )
{
	/* NOTE: we assume that d->kids is one Timer followed by trees to delete */
	
	Event e = EventMake(), f = EventMake();
	EventInclude( &e, TtExec( TtListFirst( ((Tt *)p)->kids ) ) ); /* timer */
	
	for ( SigList *i = e.sigs; !SigListEmpty( i ); i = SigListRest( i ) ) {
		switch ( SigListFirst( i ) )
		{
		case TIMER_ZERO:
			for ( TtList *i = TtListRest( p->kids ); !TtListEmpty( i ); i = TtListRest( i ) )
				TtDestroy( TtListFirst( i ) );
			
			TtDeactivateChild( p, TtListFirst( ((Tt *)p)->kids ) ); /* timer */
			                       
			SigListAdd( &f.sigs, DAMAGER_PROJ );
			
			break;
		}
	}
	
	return f;
}

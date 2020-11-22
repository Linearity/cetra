/* target display */

#include "dis-surface-data.h"
#include "embedding-data.h"

Displayer dis_target;

Event display_target( Tt *target )
{
	Event e = EventMake();
	
	for ( TtList *i = target->kids; !TtListEmpty( i ); i = TtListRest( i ) )
		EventInclude( &e, TtExec( TtListFirst( i ) ) );
	
	return NULL_EVENT;
}

void dis_target_init()
{
	DisplayerMake( &dis_target );
	
	dis_target.super.task = display_target;
	
	dis_surface_init();
	embedding_init();
	
	TtActivateChild( (Tt *)&dis_target, (Tt *)&dis_surface );
	TtActivateChild( (Tt *)&dis_target, (Tt *)&embedding );
}

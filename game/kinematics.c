#include <stdio.h>
#include "tt.h"



Event integrate_kinematics( Tt *kinematics )
{
	Event e = EventMake();
	
	for ( TtList *i = kinematics->kids; !TtListEmpty( i ); i = TtListRest( i ) )
		EventInclude( &e, TtExec( TtListFirst( i ) ) );
	
	return NULL_EVENT;
}

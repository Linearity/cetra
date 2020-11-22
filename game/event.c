#include <string.h>
#include "event.h"
#include "list.h"



SigList *SigListMake()
{
	return (SigList *)LinkedListMake();
}



Signal SigListFirst( SigList *list )
{
	return (Signal)LinkedListFirst( list );
}



SigList *SigListRest( SigList *list )
{
	return (SigList *)LinkedListRest( list );
}



int SigListEmpty( SigList *list )
{
	return LinkedListEmpty( list );
}



void SigListAdd( SigList **list_ptr, Signal s )
{
	LinkedListAdd( list_ptr, (void *)s );
	
	return;
}



void SigListRemove( SigList **list_ptr, Signal s )
{
	LinkedListRemove( list_ptr, (void *)s );
	
	return;
}



Event EventMake()
{
	Event e;
	
	e.sigs = SigListMake();
	
	return e;
}



void EventInclude( Event *including, Event included )
{
	for ( SigList *s = included.sigs; !SigListEmpty( s ); s = SigListRest( s ) )
		SigListAdd( &including->sigs, SigListFirst( s ) );
	
	return;
}



Event g_null_event = { NULL };

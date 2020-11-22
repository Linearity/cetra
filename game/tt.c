#include <assert.h>
#include <malloc.h>

#include "list.h"
#include "tt.h"



TtList *TtListMake()
{
	return (TtList *)LinkedListMake();
}



void TtListDestroy( TtList *list )
{
	if ( !TtListEmpty( list ) )
	{
		TtListDestroy( TtListRest( list ) );
		free( list );
	}
	
	return;
}



Tt *TtListFirst( TtList *list )
{
	return (Tt *)LinkedListFirst( list );
}



TtList *TtListRest( TtList *list )
{
	return (TtList *)LinkedListRest( list );
}



int TtListEmpty( TtList *list )
{
	return LinkedListEmpty( list );
}



void TtListAdd( TtList **list_ptr, Tt *c )
{
	LinkedListAdd( list_ptr, c );
	
	return;
}



void TtListRemove( TtList **list_ptr, Tt *c )
{
	LinkedListRemove( list_ptr, c );
	
	return;
}



void TtMake( Tt *me )
{
	me->task = NULL;
	me->kids = TtListMake();
	
	return;
}



void TtDestroy( Tt *me )
{
	TtListDestroy( me->kids );
	free( me );
}



Event TtExec( Tt *me )
{
	assert( me->task != NULL );
	return (*me->task)( me );
}



void TtActivateChild( Tt *me, Tt *c )
{
	TtListRemove( &me->kids, c );
	TtListAdd( &me->kids, c );
	
	return;
}



void TtDeactivateChild( Tt *me, Tt *c )
{
	TtListRemove( &me->kids, c );
	
	return;
}



Event null_task( Tt *me )
{
	return NULL_EVENT;
}
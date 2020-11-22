#ifndef __TT_H__
#define __TT_H__

#include "event.h"

typedef struct TaskTree Tt;
typedef struct LinkedList TtList;
typedef Event (*Task)(Tt *);

struct TaskTree     /* TaskTree base class */
{
	Task task;		/* the current task */
	TtList *kids;	/* the current kids */
};

TtList *TtListMake();
void TtListDestroy( TtList *list );
Tt *TtListFirst( TtList *list );
TtList *TtListRest( TtList *list );
int TtListEmpty( TtList *list );
void TtListAdd( TtList **list_ptr, Tt *c );
void TtListRemove( TtList **list_ptr, Tt *c );

void TtMake( Tt *me );
void TtDestroy( Tt *me );
Event TtExec( Tt *me );
void TtActivateChild( Tt *me, Tt *c );
void TtDeactivateChild( Tt *me, Tt *c );

Event null_task( Tt *me );
#define NULL_TASK null_task

#endif
#include <malloc.h>
#include "list.h"



/* an empty list */
LinkedList *LinkedListMake()
{
	return NULL;
}



/* the first element of the list */
void *LinkedListFirst( LinkedList *l )
{
	if ( l != NULL )
		return l->element;
	else
		return NULL;
}



/* the remainder of the list */
LinkedList *LinkedListRest( LinkedList *l )
{
	if ( l != NULL )
		return l->next;
	else
		return NULL;
}



/* whether the list is empty */
int LinkedListEmpty( LinkedList *l )
{
	return l == NULL;
}



/* create a new list element and add it to a list */
void LinkedListAdd( LinkedList **list_ptr, void *e )
{
	LinkedList *newList = (LinkedList *)malloc( sizeof( LinkedList ) );

	newList->next = *list_ptr;
	newList->element = e;
	
	*list_ptr = newList;
	
	return;
}



/* find a list element in a list, remove it, and destroy it */
void LinkedListRemove( LinkedList **list_ptr, void *e )
{
	LinkedList *iter = *list_ptr;

	if ( iter == NULL )
		return;

	if ( iter->element == e )
	{
		*list_ptr = iter->next;
		free( iter );
		return;
	}

	while ( iter->next != NULL )
	{
		if ( iter->next->element == e )
		{
			LinkedList *temp = iter->next;
			iter->next = iter->next->next;
			free( temp );
			return;
		}
		else
			iter = iter->next;
	}
	
	return;
}

#ifndef __LIST_H__
#define __LIST_H__

typedef struct LinkedList LinkedList;

struct LinkedList
{
	void *element;
	LinkedList *next;
};

LinkedList *LinkedListMake();
void *LinkedListFirst( LinkedList *l );
LinkedList *LinkedListRest( LinkedList *l );
int LinkedListEmpty( LinkedList *l );
void LinkedListAdd( LinkedList **list_ptr, void *e );
void LinkedListRemove( LinkedList **list_ptr, void *e );

#endif

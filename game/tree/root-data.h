/* root */
	
#include "scene-data.h"

Tt root;

Event update_game( Tt *root )
{
	TtDeactivateChild( &scene, (Tt *)&camera );
	
	//g_dt = (g_dt > 1.0 / 30.0) ? (1.0 / 30.0) : (g_dt);
	
	double l_dt = g_dt;
	
	while ( l_dt > 1.0 / 30.0 )
	{
		l_dt -= (g_dt = 1.0 / 30.0 );
		for ( TtList *i = root->kids; !TtListEmpty( i ); i = TtListRest( i ) )
			TtExec( TtListFirst( i ) );
	}
	
	g_dt = l_dt;
	
	TtActivateChild( &scene, (Tt *)&camera );
	
	for ( TtList *i = root->kids; !TtListEmpty( i ); i = TtListRest( i ) )
		TtExec( TtListFirst( i ) );	
	
	return NULL_EVENT;
}

void root_init()
{
	TtMake( &root );
	
	root.task = update_game;
	
	scene_init();
	
	TtActivateChild( &root, (Tt *)&scene );
}

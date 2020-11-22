#include <math.h>
#include <string.h>
#include <GL/gl.h>

#include "camera.h"
#include "tt.h"

extern double g_pi;

/* set up OpenGL state, with lights */
static void opengl_init();

static GLfloat light_position[4] = { 1.0f, 15.0f, 10.0f, 0.0f };
static GLfloat light_direction[3] = { -1.0f, -15.0f, -5.0f };



void CameraMake( Camera *c )
{
	TtMake( (Tt *)c );
	c->x = NULL;
	c->rotations = NULL;
	
	opengl_init();
	
	return;
}



Event render( Camera *camera )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode( GL_PROJECTION ) ;
	glLoadIdentity();
	glFrustum( -0.128f, 0.128f, -0.072f, 0.072f, 0.2f, 50.0f );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glRotatef( -camera->rotations[0] / (2.0 * g_pi) * 360.0, 1.0f, 0.0f, 0.0f );
	glRotatef( -camera->rotations[1] / (2.0 * g_pi) * 360.0, 0.0f, 1.0f, 0.0f );
	glRotatef( -camera->rotations[2] / (2.0 * g_pi) * 360.0, 0.0f, 0.0f, 1.0f );
	glTranslatef( -camera->x[0], -camera->x[1], -camera->x[2] );
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
	
	Event e;
	
	/* execute displayers */
	for ( TtList *i = ((Tt *)camera)->kids; !TtListEmpty( i ); i = TtListRest( i ) )
	{
		EventInclude( &e, TtExec( TtListFirst( i ) ) );
	}
	
	glFlush();
	
	return NULL_EVENT;
}



void opengl_init()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	//glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );
	glLightfv( GL_LIGHT0, GL_POSITION, light_position) ;
	glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, light_direction );
	glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 15.5 );
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
	//glEnable( GL_BLEND );
	
	return;
}

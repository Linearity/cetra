#include <assert.h>
#include <fstream>
#include <GL/glut.h>
#include <pthread.h>
#include <slMatrix.H>
#include <slUtil.H>
#include <slVector.H>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <TetMesh.h>

#include "barymap.h"
#include "camera.h"
#include "collider.h"
#include "collisions.h"
#include "deformation.h"
#include "deformer.h"
#include "displayer.h"
#include "embedding.h"
#include "fire.h"
#include "health.h"
#include "kinematics.h"
#include "kinmover.h"
#include "projtracker.h"
#include "timer.h"
#include "tt.h"
#include "vector.h"

double g_dt;
double g_grav_acc[3] = {0.0, -9.8, 0.0};
double g_pi = 3.1415926;

char g_mouseHeld;
int g_mouseLastX, g_mouseLastY;
double g_mouseDragX, g_mouseDragY;
char g_key[256];

#include "tree/root-data.h"

struct timeval lasttime, currenttime;

pthread_t tid;



void *render_thread(void *)
{
	while ( 1 )
		TtExec( (Tt *)&camera );
}



void displayCB()
{
	gettimeofday(&currenttime, NULL);
	g_dt =
		((currenttime.tv_sec - lasttime.tv_sec)	* 1000000.0
			+ (currenttime.tv_usec - lasttime.tv_usec))
		/ 1000000.0;
	lasttime = currenttime;
	
	//g_dt *= 0.5; /* run in slow-motion */
	
	TtExec( &root );     /* execute */
	
	glutSwapBuffers(); // swap buffers
}



void idleCB()
{
	g_mouseDragX = 0.0; 
	g_mouseDragY = 0.0;
	glutPostRedisplay();
}



void motionCB( int x, int y )
{
	if ( x < 50 || x > 590 || y < 50 || y > 430 )
	{
		g_mouseDragX = 0.0;
		g_mouseDragY = 0.0;
		
		glutWarpPointer( 320, 240 );
		g_mouseLastX = 320;
		g_mouseLastY = 240;
	}
	else
	{
		g_mouseDragX = (double)(x - g_mouseLastX) / 640.0;
		g_mouseDragY = (double)(y - g_mouseLastY) / 480.0;
	
		g_mouseLastX = x;
		g_mouseLastY = y;
	}
	
	glutPostRedisplay();
}



void mouseCB( int button, int state, int x, int y )
{
	switch ( button )
	{
	case GLUT_LEFT_BUTTON:
		switch ( state )
		{
		case GLUT_DOWN:
			g_mouseHeld = 1;
			break;
			
		case GLUT_UP:
			if ( g_mouseHeld )
			{
				g_mouseHeld = 0;
			}
			break;
		}
		break;
		
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		break;
	}
	glutPostRedisplay();
}



void keyboardCB(unsigned char key, int x, int y)
{
    if ( key == 27 ) {
        exit(0);
    }

    g_key[key] = 1;
}

void keyboardUpCB(unsigned char key, int x, int y)
{
    g_key[key] = 0;
}


	
int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 1280, 720 );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( argv[0] );
	
	glutIdleFunc( idleCB );
	glutDisplayFunc( displayCB );
	glutMotionFunc( motionCB );
	glutPassiveMotionFunc( motionCB );
	glutMouseFunc( mouseCB );
	glutKeyboardFunc( keyboardCB );
    glutKeyboardUpFunc( keyboardUpCB );
    
	glutSetCursor(GLUT_CURSOR_NONE);
	
	root_init();
	
	pthread_create( &tid, NULL, render_thread, NULL );
	
	gettimeofday( &lasttime, NULL );
	gettimeofday( &currenttime, NULL );
	
	g_mouseLastX = -1;
	g_mouseLastY = -1;
	
	glutMainLoop();
	return 0;
}

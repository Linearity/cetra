/*
WHY I'M WRITING THIS FILE:

I'm writing this file to fuck around in because I don't remember how to do finite elements.

by Alex Stuart 'Linearity'
*/

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "easyBMP/EasyBMP.h"
#include "IndexTet.h"
#include "slUtil.H"
#include "slVector.H"

/************************/
/*** GLOBAL VARIABLES ***/
/************************/

std::vector<SlVector3> file_vertices;
std::vector<SlTri> file_triangles;
SlVector3 *obj_vertices;
int vertex_count = 0;
SlTri *obj_faces;
int face_count = 0;
SlVector3 *node_vertices;
int 
std::string filename_base;
char *iteration_filename;
int iterations;
int current_iter;

GLfloat light_position[4];
GLfloat light_direction[3];

GLfloat wall_color[4];
GLfloat object_color[4];

int mouseClickX = 0;
int mouseClickY = 0;
char mouseHeld = 0;

GLfloat rotationAngleX = 0.0;
GLfloat rotationAngleY = 0.0;
GLfloat rotationAngleZ = 0.0;

GLfloat translationX = 0.0f;
GLfloat translationY = 0.0f;
GLfloat translationZ = -7.0f;

bool output = false;


/***************************/
/*** FUNCTION PROTOTYPES ***/
/***************************/

void displayCB();
void timerCB(int value);
void motionCB(int x, int y);
void mouseCB(int button, int state, int x, int y);



/*********************/
/*** MAIN FUNCTION ***/
/*********************/

int main(int argc, char **argv)
{
	// check for necessary argument count
	if (argc < 3)
	{
		return -1;
	}
	
	// check for flags
	for (int i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-o") == 0)
		{
			output = true;
		}
	}
	
	// READ OBJ FILE
	filename_base = std::string(argv[1]);
	iterations = atoi(argv[2]);
	iteration_filename = (char *)malloc( filename_base.size() + 2 + 5 + 4 );
	bool success = readObjFile(const_cast<char *>((filename_base + "_i00000.obj").c_str()), file_vertices, file_triangles);
	if (!success)
	{
		fprintf(stderr, "(opening input mesh file) error: unable to read OBJ file.\n");
		return false;
	}
	
	vertex_count = file_vertices.size();
	face_count = file_triangles.size();
	
	obj_vertices = (SlVector3 *)calloc(vertex_count, sizeof(SlVector3));
	obj_faces = (SlTri *)calloc(face_count, sizeof(SlTri));
	
	for (int i = 0; i < vertex_count; i++)
	{
		obj_vertices[i] = file_vertices[i];
	}
	
	for (int i = 0; i < face_count; i++)
	{
		obj_faces[i] = file_triangles[i];
	}
	
	// graphics setup
	light_position[0] = 1.0f;
	light_position[1] = 15.0f;
	light_position[2] = 10.0f;
	light_position[3] = 0.0f;
	
	light_direction[0] = -1.0f;
	light_direction[1] = -15.0f;
	light_direction[2] = -5.0f;
	
	wall_color[0] = 0.5f;
	wall_color[1] = 0.05f;
	wall_color[2] = 0.15f;
	wall_color[3] = 1.0f;
	
	object_color[0] = 0.65f;
	object_color[1] = 0.0f;
	object_color[2] = 0.9f;
	object_color[3] = 1.0f;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 300);
	glutCreateWindow("Kids, you are not important.");
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15.5);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glutDisplayFunc(displayCB);
	glutTimerFunc(33, timerCB, 0);
	glutMotionFunc(motionCB);
	glutMouseFunc(mouseCB);	
	
	
	
	current_iter = 0;
	
	// main loop
	glutMainLoop();	
	
	free(obj_vertices);
	free(obj_faces);
	
	return 0;
}



/********************************/
/*** FUNCTION IMPLEMENTATIONS ***/
/********************************/

void displayCB()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.8f, 0.8f, -0.8f, 0.8f, 1.0f, 50.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translationX, translationY, translationZ);
	glRotatef(rotationAngleX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotationAngleY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotationAngleZ, 0.0f, 0.0f, 1.0f);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
	
	SlVector3 currentNormal;	// we need to normalize this
	SlVector3 *currentVertex;	// in contrast, we only need to read this
	SlVector3 a, b;
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wall_color);
	/*
	glBegin(GL_QUADS);

		// floor
		glNormal3f(0.0f, 1.0f, 0.0f);

		glVertex3f(-5.0f, 0.0f, -5.0f);
		glVertex3f(5.0f, 0.0f, -5.0f);
		glVertex3f(5.0f, 0.0f, 5.0f);
		glVertex3f(-5.0f, 0.0f, 5.0f);

		// left wall
		glNormal3f(1.0f, 0.0f, 0.0f);

		glVertex3f(-5.0f, 0.0f, 5.0f);
		glVertex3f(-5.0f, 10.0f, 5.0f);
		glVertex3f(-5.0f, 10.0f, -5.0f);
		glVertex3f(-5.0f, 0.0f, -5.0f);

		// back wall
		glNormal3f(0.0f, 0.0f, 1.0f);

		glVertex3f(-5.0f, 0.0f, -5.0f);
		glVertex3f(-5.0f, 10.0f, -5.0f);
		glVertex3f(5.0f, 10.0f, -5.0f);
		glVertex3f(5.0f, 0.0f, -5.0f);

	glEnd();
	*/
	
	// render every triangle...for now
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, object_color);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
		for (int i = 0; i != face_count; i++)
		{
			a = obj_vertices[obj_faces[i][1]] - obj_vertices[obj_faces[i][0]];
			b = obj_vertices[obj_faces[i][2]] - obj_vertices[obj_faces[i][0]];
			
			currentNormal = cross(a, b) / mag(cross(a, b));
	
			glNormal3f((GLfloat)currentNormal.x(), (GLfloat)currentNormal.y(), (GLfloat)currentNormal.z());
			
			currentVertex = &obj_vertices[obj_faces[i][0]];
			glVertex3f((GLfloat)currentVertex->x(), (GLfloat)currentVertex->y(), (GLfloat)currentVertex->z());
			
			currentVertex = &obj_vertices[obj_faces[i][1]];
			glVertex3f((GLfloat)currentVertex->x(), (GLfloat)currentVertex->y(), (GLfloat)currentVertex->z());
			
			currentVertex = &obj_vertices[obj_faces[i][2]];
			glVertex3f((GLfloat)currentVertex->x(), (GLfloat)currentVertex->y(), (GLfloat)currentVertex->z());
		}
	glEnd();	
	glPopMatrix();
	
	glFlush();
	glPopMatrix();
	glutSwapBuffers();
}

void timerCB(int value)
{
	current_iter = (current_iter + 1) % iterations;
	
	file_vertices.clear();
	file_triangles.clear();
	
	sprintf(iteration_filename, "%s_i%05d.obj", filename_base.c_str(), current_iter);
	bool success = readObjFile(iteration_filename, file_vertices, file_triangles);
	if (!success)
	{
		fprintf(stderr, "(opening input mesh file) error: unable to read OBJ file %s.\n", iteration_filename);
		return;
	}
	
	for (int i = 0; i < vertex_count; i++)
	{
		obj_vertices[i] = file_vertices[i];
	}
	
	for (int i = 0; i < face_count; i++)
	{
		obj_faces[i] = file_triangles[i];
	}

	
	glutTimerFunc(33, timerCB, 0);
	glutPostRedisplay();
}

void motionCB(int x, int y)
{
	if (mouseHeld)
	{
		rotationAngleX += (float)(y - mouseClickY) / 480.0 * 180.0;
		rotationAngleY += (float)(x - mouseClickX) / 640.0 * 180.0;
		mouseClickX = x;
		mouseClickY = y;
	}
	glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			switch (state)
			{
				case GLUT_DOWN:
					mouseClickX = x;
					mouseClickY = y;
					mouseHeld = 1;
					break;
					
				case GLUT_UP:
					if (mouseHeld)
					{
						mouseHeld = 0;
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


/* 
 * main for exhibiting differnt join styles
 *
 * FUNCTION:
 * This demo demonstrates the various join styles,
 * and how they get applied.
 *
 * HISTORY:
 * Linas Vepstas March 1995
 * Copyright (c) 1995 Linas Vepstas <linas@linas.org>
 */


/* required include files */
#include <stdlib.h>

#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <GL/gle.h>
//#include "main.h"
//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe 
// (email Richard Campbell at ulmont@bellsouth.net)
//
#include <unistd.h>     // Header file for sleeping.
#include "PhyxEngine.h"

/* ascii code for the escape key */
#define ESCAPE 27
#define KEY_W 119
#define KEY_S 115
#define KEY_A 97
#define KEY_D 100

typedef enum {
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
} DIRECTION;

/* The number of our GLUT window */
int window; 
Pixy::PhyxEngine* mPhyxEngine;

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// Reset The Projection Matrix
	
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
	glFrustum (-9.0, 9.0, -9.0, 9.0, 50.0, 150.0);
	
	glMatrixMode(GL_MODELVIEW);
	
	/* set up a light */
	GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
	GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0}; 
	
	GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
	GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0}; 
	
	/* initialize lighting */
	glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
	glEnable (GL_LIGHT0);
	glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
	glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHTING);
	glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable (GL_COLOR_MATERIAL);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
	if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
		Height=1;
	
	glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
}

GLfloat lastx = -35.0;
GLfloat lasty = 120.0;	

float velocity = 0.0;

/* The main drawing function. */
void DrawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset The View
	
	glColor3f (0.1, 0.1, 0.1);
	
	glTranslatef(0, 0, -80.0);

	
	/* set up some matrices so that the object spins with the mouse */
	glPushMatrix ();
	glTranslatef (0.0, 0.0, 0.0);
	//glRotatef (lastx, 0.0, 1.0, 0.0);
	//glRotatef (lasty, 1.0, 0.0, 0.0);
	
	glutSolidTorus(5, 15.0, 18, 36);

	mPhyxEngine->update(0);	
	
	glPopMatrix ();
	
	// since this is double buffered, swap the buffers to display what just got drawn.
	glutSwapBuffers();

}

void moveSphere(DIRECTION);

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);
	
    /* If escape is pressed, kill everything. */
    switch (key) {
		case ESCAPE:
			/* shut down our window */
			glutDestroyWindow(window); 
			
			mPhyxEngine->cleanup();
			mPhyxEngine = NULL;
			/* exit the program...normal termination. */
			exit(0);			
			break;
		case KEY_W:
			moveSphere(FORWARD);
			break;
		case KEY_S:
			moveSphere(BACKWARD);
			break;
		default:
			break;
	}
}

/* get notified of mouse motions */
void mouseMotion (int x, int y)
{
	lastx = x;
	lasty = y;
	glutPostRedisplay ();
}

void moveSphere(DIRECTION inDir) {
	switch(inDir) {
		case FORWARD:
			velocity += 1;
			break;
		case BACKWARD:
			velocity -= 1;
			break;
	}
}

int main(int argc, char **argv) 
{
	mPhyxEngine = Pixy::PhyxEngine::getSingletonPtr();
	
	mPhyxEngine->setup();
	
	/* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
     X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */  
	glutInit(&argc, argv);  
	
	/* Select type of Display mode:   
     Double buffer 
     RGBA color
     Alpha components supported 
     Depth buffer */  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
	
	/* get a 640 x 480 window */
	glutInitWindowSize(640, 480);  
	
	/* the window starts at the upper left corner of the screen */
	glutInitWindowPosition(0, 0);  
	
	/* Open a window */  
	window = glutCreateWindow("Vertigo");  
	
	/* Register the function to do all our OpenGL drawing. */
	glutDisplayFunc(&DrawGLScene);  
	
	/* Go fullscreen.  This is as soon as possible. */
	//glutFullScreen();
	
	/* Even if there are no events, redraw our gl scene. */
	glutIdleFunc(&DrawGLScene);
	
	/* Register the function called when our window is resized. */
	glutReshapeFunc(&ReSizeGLScene);
	
	/* Register the function called when the keyboard is pressed. */
	glutKeyboardFunc(&keyPressed);
	glutMotionFunc (&mouseMotion);
	
	/* Initialize our window. */
	InitGL(640, 480);
	
	
	/* Start Event Processing Engine */  
	glutMainLoop();  
	
	return 1;
}


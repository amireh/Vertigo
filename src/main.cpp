
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

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/gle.h>
//#include "main.h"


extern float lastx;
extern float lasty;

/* ------------------------------------------------------- */

/* the arrays in which we will store the contour */
#define NCONTOUR 100
double contour_points [NCONTOUR][2];
int cidx = 0;

/* some utilities for filling that array */
#define C_PNT(x,y) { 			\
   contour_points[cidx][0] = x;		\
   contour_points[cidx][1] = y; 	\
   cidx ++;				\
}

/* the arrays in which we will store out polyline */
#define NPTS 26
double radii [NPTS];
double points [NPTS][3];
int idx = 0;

/* some utilities for filling that array */
#define PNT(x,y,z) { 			\
   points[idx][0] = x; 			\
   points[idx][1] = y; 			\
   points[idx][2] = z;			\
   idx ++;				\
}

#define RAD(r) {			\
   radii[idx] = r;			\
}
void InitStuff (void) 
{
   /* initialize the join style here */
   gleSetJoinStyle (TUBE_NORM_PATH_EDGE | TUBE_JN_ANGLE );

   RAD (4.1);
   PNT (-4.9, 12.0, 0.0);

   RAD (0.3);
   PNT (-4.8, 5.8, 0.0);

   RAD (0.3);
   PNT (-3.8, 5.8, 0.0);

   RAD (0.6);
   PNT (-3.5, 6.0, 0.0);

   RAD (0.8);
   PNT (-3.0, 7.0, 0.0);

   RAD (0.9);
   PNT (-2.4, 7.6, 0.0);

   RAD (1.0);
   PNT (-1.8, 7.6, 0.0);

   RAD (1.1);
   PNT (-1.2, 7.1, 0.0);

   RAD (1.2);
   PNT (-0.8, 5.1, 0.0);

   RAD (1.7);
   PNT (-0.3, -2.0, 0.0);

   RAD (1.8);
   PNT (-0.2, -7.0, 0.0);

   RAD (2.0);
   PNT (0.3, -7.8, 0.0);

   RAD (2.1);
   PNT (0.8, -8.2, 0.0);

   RAD (2.25);
   PNT (1.8, -8.6, 0.0);

   RAD (2.4);
   PNT (3.6, -8.6, 0.0);

   RAD (2.5);
   PNT (4.5, -8.2, 0.0);

   RAD (2.6);
   PNT (4.8, -7.5, 0.0);

   RAD (2.7);
   PNT (5.0, -6.0, 0.0);

   RAD (3.2);
   PNT (6.4, -2.0, 0.0);

   RAD (4.1);
   PNT (6.9, -1.0, 0.0);

   RAD (4.1);
   PNT (7.8, 0.5, 0.0);

}

/* draw the polycone shape */
void DrawStuff (void) 
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* set up some matrices so that the object spins with the mouse */
   glPushMatrix ();
   glTranslatef (0.0, 0.0, -80.0);
   glRotatef (lastx, 0.0, 1.0, 0.0);
   glRotatef (lasty, 1.0, 0.0, 0.0);
   glColor3f (0.5, 0.5, 0.2);

   /* Phew. FINALLY, Draw the polycone  -- */
   glePolyCone (idx, points, 0x0, radii);

   glPopMatrix ();

   glutSwapBuffers ();
}

/* --------------------------- end of file ------------------- */
/* most recent mouse position */

#ifndef NULL
#define NULL ((void *) 0x0)
#endif /* NULL */

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


float lastx=0;
float lasty=0;

/* get notified of mouse motions */
static void 
MouseMotion (int x, int y)
{
   lastx = x;
   lasty = y;
   glutPostRedisplay ();
}

static void 
JoinStyle (int msg) 
{
   int style;
   /* get the current joint style */
   style = gleGetJoinStyle ();

   /* there are four different join styles, 
    * and two different normal vector styles */
   switch (msg) {
      case 0:
         style &= ~TUBE_JN_MASK;
         style |= TUBE_JN_RAW;
         break;
      case 1:
         style &= ~TUBE_JN_MASK;
         style |= TUBE_JN_ANGLE;
         break;
      case 2:
         style &= ~TUBE_JN_MASK;
         style |= TUBE_JN_CUT;
         break;
      case 3:
         style &= ~TUBE_JN_MASK;
         style |= TUBE_JN_ROUND;
         break;

      case 20:
         style &= ~TUBE_NORM_MASK;
         style |= TUBE_NORM_FACET;
         break;
      case 21:
         style &= ~TUBE_NORM_MASK;
         style |= TUBE_NORM_EDGE;
         break;

      case 99:
         exit (0);

      default:
         break;
   }
   gleSetJoinStyle (style);
   glutPostRedisplay ();
}

/* set up a light */
GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0}; 

GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0}; 


int
main (int argc, char * argv[]) 
{
   /* initialize glut */
   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutCreateWindow ("Vertigo");
   glutDisplayFunc (DrawStuff);
   glutMotionFunc (MouseMotion);

   /* create popup menu */
   glutCreateMenu (JoinStyle);
   glutAddMenuEntry ("Raw Join Style", 0);
   glutAddMenuEntry ("Angle Join Style", 1);
   glutAddMenuEntry ("Cut Join Style", 2);
   glutAddMenuEntry ("Round Join Style", 3);
   glutAddMenuEntry ("------------------", 9999);
   glutAddMenuEntry ("Facet Normal Vectors", 20);
   glutAddMenuEntry ("Edge Normal Vectors", 21);
   glutAddMenuEntry ("------------------", 9999);
   glutAddMenuEntry ("Exit", 99);
   glutAttachMenu (GLUT_MIDDLE_BUTTON);

   /* initialize GL */
   glClearDepth (1.0);
   glEnable (GL_DEPTH_TEST);
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);

   glMatrixMode (GL_PROJECTION);
   /* roughly, measured in centimeters */
   glFrustum (-9.0, 9.0, -9.0, 9.0, 50.0, 150.0);
   glMatrixMode(GL_MODELVIEW);

   /* initialize lighting */
   glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
   glEnable (GL_LIGHT0);
   glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
   glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
   glEnable (GL_LIGHT1);
   glEnable (GL_LIGHTING);
   glEnable (GL_NORMALIZE);
   glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
   glEnable (GL_COLOR_MATERIAL);

   InitStuff ();

   glutMainLoop ();
   return 0;             /* ANSI C requires main to return int. */
}
/* ------------------ end of file -------------------- */

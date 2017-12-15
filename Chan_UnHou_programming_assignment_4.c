/*Un Hou Chan*/
//Reference: http://www3.ntu.edu.sg/home/ehchua/programming/opengl/cg_introduction.html

//OpenGL include thingy//
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

typedef enum {RECT_SHAPE, ELLIP_SHAPE, LINE_SHAPE, BEZIER_SHAPE} Shape_type;
typedef enum {DRAW_FILL, DRAW_LINE} Filled_type;

/*
   2D Point w/ GLfloat
 */
struct Point2D {
        GLfloat x;
        GLfloat y;
};
/*
   shape: RECT_SHAPE, ELLIP_SHAPE, LINE_SHAPE, BEZIER_SHAPE
   filled: DRAW_FILL, DRAW_LINE; relevant only to RECT_SHAPE, ELLIP_SHAPE
   p3, p4 are only relevant to BEZIER_SHAPE
 */
struct Obj {
        Shape_type shape;
        Filled_type filled;
        struct Point2D p1;
        struct Point2D p2;
        struct Point2D p3;
        struct Point2D p4;
};
/*
   Stack node of ObjStack
 */
struct ObjNode {
        struct Obj obj;
        struct ObjNode* next;
};
/*
   Experimental: Obj Stack for undo action
 */
struct ObjStack {
        struct ObjNode* top;
};

void drawObj(){
  glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
      glColor3f(1.0f, 0.0f, 0.0f); // Red
      glVertex2f(-0.8f, 0.1f);     // Define vertices in counter-clockwise (CCW) order
      glVertex2f(-0.2f, 0.1f);     //  so that the normal (front-face) is facing you
      glVertex2f(-0.2f, 0.7f);
      glVertex2f(-0.8f, 0.7f);

      glColor3f(0.0f, 1.0f, 0.0f); // Green
      glVertex2f(-0.7f, -0.6f);
      glVertex2f(-0.1f, -0.6f);
      glVertex2f(-0.1f,  0.0f);
      glVertex2f(-0.7f,  0.0f);

      glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
      glVertex2f(-0.9f, -0.7f);
      glColor3f(1.0f, 1.0f, 1.0f); // White
      glVertex2f(-0.5f, -0.7f);
      glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
      glVertex2f(-0.5f, -0.3f);
      glColor3f(1.0f, 1.0f, 1.0f); // White
      glVertex2f(-0.9f, -0.3f);
   glEnd();

   glBegin(GL_TRIANGLES);          // Each set of 3 vertices form a triangle
      glColor3f(0.0f, 0.0f, 1.0f); // Blue
      glVertex2f(0.1f, -0.6f);
      glVertex2f(0.7f, -0.6f);
      glVertex2f(0.4f, -0.1f);

      glColor3f(1.0f, 0.0f, 0.0f); // Red
      glVertex2f(0.3f, -0.4f);
      glColor3f(0.0f, 1.0f, 0.0f); // Green
      glVertex2f(0.9f, -0.4f);
      glColor3f(0.0f, 0.0f, 1.0f); // Blue
      glVertex2f(0.6f, -0.9f);
   glEnd();

   glBegin(GL_POLYGON);            // These vertices form a closed polygon
      glColor3f(1.0f, 1.0f, 0.0f); // Yellow
      glVertex2f(0.4f, 0.2f);
      glVertex2f(0.6f, 0.2f);
      glVertex2f(0.7f, 0.4f);
      glVertex2f(0.6f, 0.6f);
      glVertex2f(0.4f, 0.6f);
      glVertex2f(0.3f, 0.4f);
   glEnd();
}

/**
 * Set "clearing" or background color to White and opaque
 */
void init(){
  glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
}

/**
 * Handler for window-repaint event. Call back when the window first appears and
 * whenever the window needs to be re-painted.
 */
void display(){
  glClear (GL_COLOR_BUFFER_BIT);
  drawObj();

  glFlush();
}

/**
 * Handler for window re-size event. Called back when the window first appears and
 * whenever the window is re-sized with its new width and height
 * @param width Window width
 * @param height Window height
 */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // // Set the aspect ratio of the clipping area to match the viewport
   // glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   // glLoadIdentity();             // Reset the projection matrix
   // if (width >= height) {
   //   // aspect >= 1, set the height from -1 to 1, with larger width
   //    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
   // } else {
   //    // aspect < 1, set the width to -1 to 1, with larger height
   //   gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
   // }
   gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}


int main(int argc, char *argv[]) {

  int window;
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (720, 720);
  glutInitWindowPosition (0, 0);
  window = glutCreateWindow ("Paint Program");
  init();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);

  glutMainLoop ();

  return 0;
}

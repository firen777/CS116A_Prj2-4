/*Un Hou Chan*/

//OpenGL include thingy//
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/glut.h>
#endif
//====================//
//Standard Lib//
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
//============//
//Constants//
#define PI 3.14159265
#define VERT_COUNT 3000
#define FACE_COUNT 5000
#define ROTATE_ANGLE 3

const char* OBJ_PATH = "bunny.obj";
//=========//

//typedef//

/* vertices:
(x,y,z) with type GLfloat.
 */
typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
} ObjVert;

/* Face:
containing 3 pointers to the ObjVert
that make up the face.
 */
typedef struct {
  ObjVert* v1;
  ObjVert* v2;
  ObjVert* v3;
} ObjFace;

/* Camera, using Spherical coordinate:

 void gluLookAt
 ( eyeX, eyeY, eyeZ,
 	 centerX, centerY, centerZ,
 	 upX, upY, upZ);

  eye(X,Y,Z) = (l*sin(th)*cos(rh), l*sin(th)*sin(rh), l*cos(th))
  center(X,Y,Z) = (0,0,0)
  up(X,Y,Z) = (0,1,0) //to be experimented


 */
typedef struct {
  GLdouble l; //distant
  GLdouble th; //theta
  GLdouble rh; //rho
} ObjCam;
//=======//

//Global objects//
ObjCam objcam = {.l=4.0, .th=0, .rh=0};
ObjVert objvert[VERT_COUNT];
ObjFace objface[FACE_COUNT];
int vCount = 0;
int fCount = 0;
GLenum gl_mode = GL_LINE;
GLfloat objscale = 1.0f;
GLfloat objYoffset= 0.0f;
//=============//

///////////////////////////////////////////////////////////////////

/**Parse the .obj file and store vertices and faces in objvert[] and objface[].
 * Global object used: objface[], objvert[], fCount, vCount.
 * objvert[]: store vertices.
 * objface[]: store faces.
 * fCount: # of faces read.
 * vCount: # of vertices read.
 * @param path path to .obj file.
 */
void parseObjFile(const char* path){
  FILE* objInput;
  char buffer [100];
  //Reference read line: http://www.cplusplus.com/reference/cstdio/FILE/
  //Reference extraction: https://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm
  objInput = fopen(path, "r");
  if (objInput == NULL) perror ("Error opening file");
  else
  {
    while (!feof(objInput))
    {
      if (fgets(buffer, 100, objInput) == NULL) break;
      if (buffer[0]=='v') {
        GLfloat x,y,z;
        sscanf(buffer, "%*s %f %f %f", &x, &y, &z);
        objvert[vCount].x = x;
        objvert[vCount].y = y;
        objvert[vCount].z = z;
        vCount++;
      }
      else if (buffer[0]=='f') {
        int v1,v2,v3;
        sscanf(buffer, "%*s %d %d %d", &v1, &v2, &v3);
        //off by one: .obj index start at 1:
        v1--; v2--; v3--;
        objface[fCount].v1 = &(objvert[v1]);
        objface[fCount].v2 = &(objvert[v2]);
        objface[fCount].v3 = &(objvert[v3]);
        fCount++;
      }
    }
    fclose(objInput);
  }
}

void init(){
  glShadeModel (GL_SMOOTH); //experiment: GL_SMOOTH
  glClearColor (0.3f, 0.3f, 0.3f, 0.0f);
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
}

/**draw the .obj;
 * Global object used: objface[], objvert[], fCount, gl_mode, objscale, objYoffset.
 * objface[]: used for rendering.
 * objvert[]: implied by using objface[].
 * fCount: keep track of how many faces exist
 * gl_mode: wireframe/solid mode.
 * objscale: used for scaling object.
 * objYoffset: used for offsetting Y position of object.
 */
void drawObj(){
  glPolygonMode(GL_FRONT_AND_BACK, gl_mode);
  glColor3f(0.0f,1.0f,0.0f);
  for (int i=0; i<fCount; i++) {
    glBegin(GL_POLYGON);
    glVertex3f(objface[i].v1->x*objscale, (objface[i].v1->y+objYoffset)*objscale, objface[i].v1->z*objscale);
    glVertex3f(objface[i].v2->x*objscale, (objface[i].v2->y+objYoffset)*objscale, objface[i].v2->z*objscale);
    glVertex3f(objface[i].v3->x*objscale, (objface[i].v3->y+objYoffset)*objscale, objface[i].v3->z*objscale);
    glEnd();
  }
}

void display(){
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  //cam setup
  /*
  eye(X,Y,Z) = (l*sin(th)*cos(rh), l*sin(th)*sin(rh), l*cos(th))
  center(X,Y,Z) = (0,0,0)
  up(X,Y,Z) = (0,1,0) //to be experimented
   */
  gluLookAt(objcam.l*sin(objcam.th)*cos(objcam.rh), objcam.l*sin(objcam.th)*sin(objcam.rh), objcam.l*cos(objcam.th), //eye position
            0.0f, 0.0f, 0.0f,  //eye looking at
            0.0f, 1.0f, 0.0f); //UP vector

  drawObj();

  glutSwapBuffers();
  glutPostRedisplay();
}

void reshape (int w, int h){
  glViewport (0, 0, w, h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  if (h == 0)
  {
    gluPerspective (80, (float) w, 1.0, 5000.0);
  }
  else
  {
    gluPerspective (80, (float) w / (float) h, 1.0, 5000.0);
  }
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

}

/**Keypress function.
 * Pressing 'w' should display the OBJ file as a wireframe mesh.
 * Pressing 's' should display the OBJ file as a solid body model.
 * NEW: 'q': scale object up
 * NEW: 'a': scale object down
 * NEW: 'e': object translate up
 * NEW: 'd': object translate down
 * NEW: 'i': cam rotate top// NOT YET DONE
 * NEW: 'k': cam rotate bottom// NOT YET DONE
 * NEW: 'r': reset cam
 * @param key key pressed
 * @param x   mouse X position, irrelevant.
 * @param y   mouse Y position, irrelevant.
 */
void keyPressed(unsigned char key,int x, int y){
  switch (key)
  {
    case 'w':
      gl_mode = GL_LINE;
    break;
    case 's':
      gl_mode = GL_FILL;
    break;
    case 'q':
      objscale += 1.0f;
    break;
    case 'a':
      objscale -= 1.0f;
      if (objscale <= 1.0f) objscale = 1.0f;
    break;
    case 'e':
      objYoffset += 0.01f;
    break;
    case 'd':
      objYoffset -= 0.01f;
    break;
    case 'i':
      objcam.rh += ROTATE_ANGLE*(2*PI)/360.0;
      objcam.rh = fmod(objcam.rh, 2*PI);
    break;
    case 'k':
      objcam.rh -= ROTATE_ANGLE*(2*PI)/360.0;
      objcam.rh = fmod(objcam.rh, 2*PI);
    break;
    case 'r':
      objcam.l = 4.0;
      objcam.th= 0.0;
      objcam.rh= 0.0;
    default:
    break;
  }
}

/**Arrow Keypress function.
 * Pressing the up-arrow key should move the camera closer to the model.
 * Pressing the down-arrow key should move the camera away from the model.
 * Pressing the left-arrow key should rotate the camera counter-clockwise.
 * Pressing the right-arrow key should rotate the camera clockwise
 * @param key key pressed
 * @param x   mouse X position, irrelevant.
 * @param y   mouse Y position, irrelevant.
 */
void arrowPressed(int key, int x, int y){
  switch (key)
  {
    case GLUT_KEY_UP:
      objcam.l -= 1.0;
      if (objcam.l <= 1.0) objcam.l = 1.0;
    break;
    case GLUT_KEY_DOWN:
      objcam.l += 1.0;
    break;
    case GLUT_KEY_LEFT:
      objcam.th += ROTATE_ANGLE*(2*PI)/360.0;
      objcam.th = fmod(objcam.th, 2*PI);
    break;
    case GLUT_KEY_RIGHT:
      objcam.th -= ROTATE_ANGLE*(2*PI)/360.0;
      objcam.th = fmod(objcam.th, 2*PI);
    break;
    default:
    break;
  }
}


int main(int argc, char *argv[]) {
  parseObjFile(OBJ_PATH);

  int window;
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize (1280, 720 );
  glutInitWindowPosition (0, 0);
  window = glutCreateWindow ("Bunny Model");

  init ();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyPressed);
  glutSpecialFunc (arrowPressed);

  glutMainLoop ();

  return 0;
}

/* Un Hou Chan */

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/glut.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <stdio.h> //debug purpose, To be deleted


#define CELL_WIDTH 2.0f
#define CELL_PER_ROW 50
#define CELL_PER_COL 25
#define TERR_Y_RANGE 2.0f
#define MOVE_DIST 1.0f

typedef struct {
  float x; //[+n , -n]
  float y; //[+m , -m]
  float z; //[+n , -n]
} TerrainVertex;

typedef struct {
  float x; //0
  float y; //some amount above ground
  float z; //0
  float r; //angle not implemented.
} TerrainCam;

//=========Global Var=============//
TerrainVertex terrainVertex[CELL_PER_ROW][CELL_PER_COL];

TerrainCam terrainCam = {.x=0, .y=3.0f, .z=0, .r=0};
//================================//

void keyPressed(unsigned char key,int x, int y){
  //see:https://www.opengl.org/resources/libraries/glut/spec3/node49.html
  //The 'a' key moves the camera and the camera focus up (further above the ground).
	//The 'z' key moves the camera and the camera focus down (closer to the ground).

	//'a'=> ypos++
	//'z'=> ypos--
  switch (key)
  {
    case 'a':
    terrainCam.y=terrainCam.y+MOVE_DIST;
    break;
    case 'z':
    terrainCam.y=terrainCam.y-MOVE_DIST;
    break;
    default:
    break;
  }


}

void arrowPressed(int key, int x, int y){
  //see:https://www.opengl.org/resources/libraries/glut/spec3/node54.html
  //The up-arrow key moves the camera and the camera focus forward.
	//The down-arrow key moves the camera and the camera focus backward.
	//The left-arrow key moves the camera and camera focus right.
	//The right-arrow key moves the camera and camera focus left.

  //up => zpos--
  //down => zpos++
  //left => xpos++...?
  //right => xpos--...?

  switch (key)
  {
    case GLUT_KEY_UP:
    if (terrainCam.z > -(float)(CELL_WIDTH*CELL_PER_COL)/2.0f +3.0f)
      terrainCam.z=terrainCam.z-MOVE_DIST;
    break;
    case GLUT_KEY_DOWN:
    if (terrainCam.z < (float)(CELL_WIDTH*CELL_PER_COL)/2.0f  +3.0f)
      terrainCam.z=terrainCam.z+MOVE_DIST;
    break;
    case GLUT_KEY_LEFT:
    if (terrainCam.x < (float)(CELL_WIDTH*CELL_PER_ROW)/2.0f  +1.0f)
      terrainCam.x=terrainCam.x+MOVE_DIST;
    break;
    case GLUT_KEY_RIGHT:
    if (terrainCam.x > -(float)(CELL_WIDTH*CELL_PER_ROW)/2.0f -1.0f)
      terrainCam.x=terrainCam.x-MOVE_DIST;
    break;
    default:
    break;
  }


}

void init(){
  glShadeModel (GL_FLAT);
  glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);

}

void drawTerrain(){
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_LINE: wireframe
  glColor3f(0.0f,0.0f,0.0f); //black pen

  /*
  for all vertices:
  polygon draw [i][j], [i-1][j], [i][j-1]
   */
  for (int i=0; i<CELL_PER_ROW-1; i++){
    glBegin(GL_POLYGON);
    glVertex3f(terrainVertex[i][0].x,terrainVertex[i][0].y,terrainVertex[i][0].z);
    glVertex3f(terrainVertex[i+1][0].x,terrainVertex[i+1][0].y,terrainVertex[i+1][0].z);
    glVertex3f(terrainVertex[i][1].x,terrainVertex[i][1].y,terrainVertex[i][1].z);
    glEnd();
  }
  for (int j=0; j<CELL_PER_COL-1; j++){
    glBegin(GL_POLYGON);
    glVertex3f(terrainVertex[0][j].x,terrainVertex[0][j].y,terrainVertex[0][j].z);
    glVertex3f(terrainVertex[1][j].x,terrainVertex[1][j].y,terrainVertex[1][j].z);
    glVertex3f(terrainVertex[0][j+1].x,terrainVertex[0][j+1].y,terrainVertex[0][j+1].z);
    glEnd();
  }
  for (int i=1; i<CELL_PER_ROW; i++){
    for (int j=1; j<CELL_PER_COL; j++){
        glBegin(GL_POLYGON);
        glVertex3f(terrainVertex[i][j].x,terrainVertex[i][j].y,terrainVertex[i][j].z);
        glVertex3f(terrainVertex[i-1][j].x,terrainVertex[i-1][j].y,terrainVertex[i-1][j].z);
        glVertex3f(terrainVertex[i][j-1].x,terrainVertex[i][j-1].y,terrainVertex[i][j-1].z);
        glEnd();
    }
  }

}

void display (void){

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  //cam setup
  gluLookAt(terrainCam.x,terrainCam.y,terrainCam.z, //eye position
            terrainCam.x,terrainCam.y-1.0f,terrainCam.z-5.0f,  //eye looking at
            0.0f, 1.0f, 0.0f); //UP vector

  drawTerrain();

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


int main(int argc, char *argv[]) {

  //RNGesus: https://stackoverflow.com/questions/686353/c-random-float-number-generation
  srand((unsigned int)time(NULL));

  for (int i=0; i<CELL_PER_ROW; i++){
    for (int j=0; j<CELL_PER_COL; j++){
        terrainVertex[i][j].x=i*CELL_WIDTH - (float)(CELL_WIDTH*CELL_PER_ROW)/2.0f;
        terrainVertex[i][j].y=(float)rand()/(float)(RAND_MAX/TERR_Y_RANGE)-TERR_Y_RANGE/2.0f;
        // terrainVertex[i][j].y=1.0f + (float)(i%2)*-2;
        terrainVertex[i][j].z=j*CELL_WIDTH - (float)(CELL_WIDTH*CELL_PER_COL)/2.0f;;

        //printf("%f\n",terrainVertex[i][j].y);
    }
  }

  int window;
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize (1280, 720 );
  glutInitWindowPosition (0, 0);
  window = glutCreateWindow ("Animating Camera Test");

  init ();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyPressed);
  glutSpecialFunc (arrowPressed);

  glutMainLoop ();

  return 0;
}

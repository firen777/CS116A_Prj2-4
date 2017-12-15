/*Un Hou Chan*/

//Reference: http://www3.ntu.edu.sg/home/ehchua/programming/opengl/cg_introduction.html

//For old version GCC: need to link library "-lstdc++" for C++ operator (like new, delete) to work.

/**
 * Shape interface can draw itself.
 * ShapeList holds all the Shape to be drawn. Experiment: implement undo function by popping.
 * StateMachine controls flow of mouse clicking and Shape instantiation.
 */

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
//=========================================================//
//Constants:
//Opcode:
#define opRECT 0x100
#define opELLP 0x200
#define opLINE 0x300
#define opBEZI 0x400

#define opFILL 0x10
#define opOUTL 0x20

#define opRED 0x1
#define opGRN 0x2
#define opBLU 0x3
#define opCYN 0x4
#define opMAG 0x5
#define opYLW 0x6
#define opBLK 0x7
#define opBWN 0x8
//MISC:
#define PI 3.14159265
#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 720
//=========================================================//

/**Declare helper subroutine. call glColor3f(...) according to color code.
 * @param color color OpCode
 */
void colorSelecter(int color);
/**Declare helper function. Angle->Rad
 * rad = angle*(2*PI)/360.0;
 * @param  angle
 * @return radian
 */
double angle2rad(int angle);

/* Point2D (x, y)
 */
struct Point2D {
  GLint x;
  GLint y;
};
/* Shape Interface has void draw() that draw itself.
 */
class Shape{
  public:
    /**draw the Shape itself.
     */
    virtual void draw()=0;
};
/* Rectangle Shape.
 */
class ShapeRect: public Shape{
  private:
    Point2D p1;
    Point2D p2;
    bool filled;
    int color;
  public:
    /**Rectangle Shape.
     * @param p1     point 1: Point2D
     * @param p2     point 2: Point2D
     * @param filled filled or outline
     * @param color color opcode
     */
    ShapeRect(Point2D p1, Point2D p2, bool filled, int color){
      this->p1 = p1;
      this->p2 = p2;
      this->filled = filled;
      this->color = color;
    }
    /**Draw the Rectangle.
     */
    void draw() {
      glBegin(filled ? GL_QUADS : GL_LINE_LOOP);
      colorSelecter(color);
      glVertex2i(p1.x, p1.y);
      glVertex2i(p1.x, p2.y);
      glVertex2i(p2.x, p2.y);
      glVertex2i(p2.x, p1.y);
      glEnd();
    }
};
/* Ellipse Shape.
 */
class ShapeEllip: public Shape{
  private:
    Point2D p1;
    Point2D p2;
    bool filled;
    int color;
  public:
    /**Ellipse Shape.
     * @param p1     point 1: Point2D
     * @param p2     point 2: Point2D
     * @param filled filled or outline
     * @param color color opcode
     */
    ShapeEllip(Point2D p1, Point2D p2, bool filled, int color){
      this->p1 = p1;
      this->p2 = p2;
      this->filled = filled;
      this->color = color;
    }
    /**Draw the Ellipse with a bounding box
     * Reference: https://softwareengineering.stackexchange.com/questions/131963/drawing-ellipse-from-a-bounding-box
     */
    void draw() {
      glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
      colorSelecter(color);
      Point2D r = {.x = (p2.x-p1.x)/2, .y = (p2.y-p1.y)/2};
      Point2D c = {.x = (p2.x+p1.x)/2, .y = (p2.y+p1.y)/2};
      int lastX = p1.x; int lastY = p1.y;
      int newX = c.x + cos(angle2rad(0)) * r.x;
      int newY = c.y + sin(angle2rad(0)) * r.y;
      lastX = newX;
      lastY = newY;

      for (int i=1; i<360; i++) {
        newX = c.x + cos(angle2rad(i)) * r.x;
        newY = c.y + sin(angle2rad(i)) * r.y;
        glVertex2i(lastX, lastY);
        glVertex2i(newX, newY);
        lastX = newX;
        lastY = newY;
      }
      glEnd();
    }
};
/* Line Shape.
 */
class ShapeLine: public Shape{
  private:
    Point2D p1;
    Point2D p2;
    int color;
  public:
    /**Line Shape.
     * @param p1     point 1: Point2D
     * @param p2     point 2: Point2D
     * @param color  color opcode
     */
    ShapeLine(Point2D p1, Point2D p2, int color){
      this->p1 = p1;
      this->p2 = p2;
      this->color = color;
    }
    /**Draw the Line with two point.
     */
    void draw() {
      glBegin(GL_LINES);
      colorSelecter(color);
      glVertex2i(p1.x, p1.y);
      glVertex2i(p2.x, p2.y);
      glEnd();
    }
};
/* Bezier Shape.
 */
class ShapeBezi: public Shape{
  private:
    Point2D pt[4];
    int color;
    /**helper function: get x coordinate given t
     * @param  t (0 to 1)
     * @return x
     */
    int xPoint(double t){
      double answer = 0.0;
      answer +=     pow(1.0-t, 3-0)         *(double)pt[0].x;
      answer += 3.0*pow(1.0-t, 3-1)*    t   *(double)pt[1].x;
      answer += 3.0*   (1.0-t     )*pow(t,2)*(double)pt[2].x;
      answer +=                     pow(t,3)*(double)pt[3].x;
      return (int)answer;
    }
    /**helper function: get y coordinate given t
     * @param  t (0 to 1)
     * @return y
     */
    int yPoint(double t){
      double answer = 0.0;
      answer +=     pow(1.0-t, 3-0)         *(double)pt[0].y;
      answer += 3.0*pow(1.0-t, 3-1)*    t   *(double)pt[1].y;
      answer += 3.0*   (1.0-t     )*pow(t,2)*(double)pt[2].y;
      answer +=                     pow(t,3)*(double)pt[3].y;
      return (int)answer;
    }
  public:
    /**Bezier curve Shape.
     * @param p1     point 1: Point2D
     * @param p2     point 2: Point2D
     * @param p3     point 3: Point2D
     * @param p4     point 4: Point2D
     * @param color  color opcode
     */
    ShapeBezi(Point2D p1, Point2D p2, Point2D p3, Point2D p4, int color) {
      this->pt[0] = p1;
      this->pt[1] = p2;
      this->pt[2] = p3;
      this->pt[3] = p4;
      this->color = color;
    }
    /**Draw the Bezier Curve with four point.
     */
    void draw() {
      glBegin(GL_LINE_STRIP);
      colorSelecter(color);
      int px, py;
      int precision=30;
      for (int i=0; i<=precision; i++){
        px = xPoint((double)i/(double)precision);
        py = yPoint((double)i/(double)precision);
        glVertex2i(px, py);
      }
      glEnd();
    }
};
//--------------------------------------------//
class ShapeNode{
  private:
    Shape* content;
  public:
    ShapeNode* next;
    ShapeNode* prv;
    /**Node to Shape Stack
     * @param content pointer to Allocated Shape
     */
    ShapeNode(Shape* content) {
      this->content = content;
      this->next = NULL;
      this->prv = NULL;
    }

    /**Delegate the draw function to Shape content.
     */
    void draw(){
      content->draw();
    }
    /**get pointer to Allocated Shape
     * @return Shape content
     */
    Shape* getContent(){
      return content;
    }
};

class ShapeList{
  private:
    int counter;
    ShapeNode* headPtr;
    ShapeNode* tailPtr;
  public:
    ShapeList(Shape* content=NULL){
      headPtr = NULL;
      tailPtr = NULL;
      counter = 0;
    }
    /**@deprecated
     * Dequeue the queue. Return NULL if empty
     * @return Shape content of head Node.
     */
    Shape* dequeue(){
      if (counter==0||headPtr==NULL||tailPtr==NULL) return NULL;

      ShapeNode* tempPtr = headPtr;
      headPtr = headPtr->prv;
      Shape* tempReturn = tempPtr->getContent();
      delete tempPtr;
      counter--;
      if (counter == 0) {
        headPtr = NULL;
        tailPtr = NULL;
      }
      else {
        headPtr->next=NULL;
      }
      return tempReturn;
    }
    /**Pop the queue. Return NULL if empty
     * @return Shape content of tail Node.
     */
    Shape* pop(){
      if (counter==0||headPtr==NULL||tailPtr==NULL) return NULL;

      ShapeNode* tempPtr = tailPtr;
      tailPtr = tailPtr->next;
      Shape* tempReturn = tempPtr->getContent();
      delete tempPtr;
      counter--;
      if (counter == 0) {
        headPtr = NULL;
        tailPtr = NULL;
      }
      else {
        tailPtr->prv=NULL;
      }
      return tempReturn;
    }

    /**Enqueue.
     * @param content pointer to allocated Shape
     */
    void enqueue(Shape* content){
      if (counter == 0 ) {
        headPtr = new ShapeNode(content);
        tailPtr = headPtr;
      }
      else {
        ShapeNode* tempPtr = tailPtr;
        tailPtr = new ShapeNode(content);
        tempPtr->prv = tailPtr;
        tailPtr->next= tempPtr;
      }
      counter++;
    }
    /**Draw the whole queue by delegating draw function to the Node.
     */
    void drawAll(){
      ShapeNode* tempPtr = headPtr;
      while (tempPtr!=NULL) {
        tempPtr->draw();
        tempPtr=tempPtr->prv;
      }
    }
};

class StateMachine{
  private:
    int state; //function like a index pointer pointing at pt[]
    int currShape, currFill, currColor;
    Point2D pt[4];
    bool drawFlag;
    /**helper function: draw crosshair
     * @param i pt[i]
     */
    void drawCrossHair(int i){
      glBegin(GL_LINES);
      colorSelecter(currColor);
      glVertex2i(pt[i].x-5, pt[i].y);
      glVertex2i(pt[i].x+5, pt[i].y);
      glVertex2i(pt[i].x, pt[i].y-5);
      glVertex2i(pt[i].x, pt[i].y+5);
      glEnd();
    }
  public:
    /**State Machine to control the flow of the draw program.
     */
    StateMachine(){
      state = 0; currShape = 0; currFill = 0; currColor = 0;
      pt[0].x = 0; pt[0].y = 0;
      pt[1].x = 0; pt[1].y = 0;
      pt[2].x = 0; pt[2].y = 0;
      pt[3].x = 0; pt[3].y = 0;
      drawFlag = false;
    }
    /**Select a new shape. Reset state in the process.
     * @param opcode data about shape, fill, color.
     */
    void setOpcode(int opcode){
      state = 0;
      currShape = opcode & 0xf00;
      currFill = opcode & 0xf0;
      currColor = opcode & 0xf;
      pt[0].x = 0; pt[0].y = 0;
      pt[1].x = 0; pt[1].y = 0;
      pt[2].x = 0; pt[2].y = 0;
      pt[3].x = 0; pt[3].y = 0;
      drawFlag = false;
    }
    /**Determines if enough data to draw shape or not.
     * if two point has been input, can draw Rectangle, Ellipse and Line.
     * if four point has been input, can draw Bezier.
     * @return shape is drawable?
     */
    bool drawAble(){
      return drawFlag;
    }
    /**add a new Point and update state.
     * @param p Point2D
     */
    void addPoint(Point2D p){
      drawFlag = false;
      if (currShape==opRECT || currShape==opELLP || currShape==opLINE) {
        pt[state].x = p.x; pt[state].y = p.y;
        state++;
        if (state == 2) {  //has 2 points.
          drawFlag = true; //can now be drawn.
          state = 0;       //back to state 0
        }
      }
      if (currShape==opBEZI) {
        pt[state].x = p.x; pt[state].y = p.y;
        state++;
        if (state == 4) {  //has 4 points.
          drawFlag = true; //can now be drawn.
          state = 0;       //back to state 0
        }
      }
    }
    /**Draw previously clicked point.
     * Draw first point for Rectangle, Ellipse, Line
     * Draw first 3 points for Bezier
     */
    void drawPoints(){
      for (int i=0; i<state; i++)
        drawCrossHair(i);
    }
    /**Return a pointer to an allocated Shape according to current state.
     * @return a Shape, NULL if invalid condition.
     */
    Shape* makeShape(){
      Shape* s = NULL;
      if (drawFlag) {
        switch (currShape) {
          case opRECT:
          s = new ShapeRect(pt[0],pt[1],currFill==opFILL,currColor);
          break;
          case opELLP:
          s = new ShapeEllip(pt[0],pt[1],currFill==opFILL,currColor);
          break;
          case opLINE:
          s = new ShapeLine(pt[0],pt[1],currColor);
          break;
          case opBEZI:
          s = new ShapeBezi(pt[0],pt[1],pt[2],pt[3],currColor);
          break;
          default:
          break;
        }
      }
      return s;
    }
};
//=========================================================//
//Global Var:
ShapeList shapeList;
StateMachine sm;
//=========================================================//

/**On Right Click down, invoke StateMachine add point function and update display.
 * @param button mouse button
 * @param state  down or up
 * @param x      x position
 * @param y      y position
 */
void mouse(int button, int state, int x, int y){
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    Point2D p = {.x=x, .y=y};
    sm.addPoint(p);
    if (sm.drawAble())
      shapeList.enqueue(sm.makeShape());
  }
  glutPostRedisplay();
}

/**helper subroutine. call glColor3f(...) according to color code.
 * @param color color OpCode
 */
void colorSelecter(int color){
  switch (color) {
    case opRED: glColor3f(1.0f, 0.0f, 0.0f);
    break;
    case opGRN: glColor3f(0.0f, 1.0f, 0.0f);
    break;
    case opBLU: glColor3f(0.0f, 0.0f, 1.0f);
    break;
    case opCYN: glColor3f(0.0f, 1.0f, 1.0f);
    break;
    case opMAG: glColor3f(1.0f, 0.0f, 1.0f);
    break;
    case opYLW: glColor3f(1.0f, 1.0f, 0.0f);
    break;
    case opBLK: glColor3f(0.0f, 0.0f, 0.0f);
    break;
    case opBWN: glColor3f(0.6f, 0.2f, 0.2f);
    break;
    default:
    break;
  }
}
/**helper function. Angle->Rad
 * rad = angle*(2*PI)/360.0;
 * @param  angle
 * @return radian
 */
double angle2rad(int angle){
  return (double)angle*PI/180.0;
}

void drawShape(){
  shapeList.drawAll();
  sm.drawPoints();
}


/**Event Handler for menu.
 * @param value menu entry
 */
void menuHandle(int value){
  // int targetShape = value & 0xf00;
  // int targetFill = value & 0xf0;
  // int targetColor = value & 0xf;
  sm.setOpcode(value);

  // printf("%d, %d, %d\n", targetShape>>8, targetFill>>4, targetColor);
}

/**Menu initialization
 */
void menuInit(){
  int rect_filled = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opRECT|opFILL|opRED);
    glutAddMenuEntry("Green", opRECT|opFILL|opGRN);
    glutAddMenuEntry("Blue", opRECT|opFILL|opBLU);
    glutAddMenuEntry("Cyan", opRECT|opFILL|opCYN);
    glutAddMenuEntry("Magenta", opRECT|opFILL|opMAG);
    glutAddMenuEntry("Yellow", opRECT|opFILL|opYLW);
    glutAddMenuEntry("Black", opRECT|opFILL|opBLK);
    glutAddMenuEntry("Brown", opRECT|opFILL|opBWN);
  int rect_outline = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opRECT|opOUTL|opRED);
    glutAddMenuEntry("Green", opRECT|opOUTL|opGRN);
    glutAddMenuEntry("Blue", opRECT|opOUTL|opBLU);
    glutAddMenuEntry("Cyan", opRECT|opOUTL|opCYN);
    glutAddMenuEntry("Magenta", opRECT|opOUTL|opMAG);
    glutAddMenuEntry("Yellow", opRECT|opOUTL|opYLW);
    glutAddMenuEntry("Black", opRECT|opOUTL|opBLK);
    glutAddMenuEntry("Brown", opRECT|opOUTL|opBWN);
  int rect = glutCreateMenu(menuHandle);
    glutAddSubMenu ("Filled", rect_filled);
    glutAddSubMenu ("Outline", rect_outline);

  int ellp_filled = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opELLP|opFILL|opRED);
    glutAddMenuEntry("Green", opELLP|opFILL|opGRN);
    glutAddMenuEntry("Blue", opELLP|opFILL|opBLU);
    glutAddMenuEntry("Cyan", opELLP|opFILL|opCYN);
    glutAddMenuEntry("Magenta", opELLP|opFILL|opMAG);
    glutAddMenuEntry("Yellow", opELLP|opFILL|opYLW);
    glutAddMenuEntry("Black", opELLP|opFILL|opBLK);
    glutAddMenuEntry("Brown", opELLP|opFILL|opBWN);
  int ellp_outline = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opELLP|opOUTL|opRED);
    glutAddMenuEntry("Green", opELLP|opOUTL|opGRN);
    glutAddMenuEntry("Blue", opELLP|opOUTL|opBLU);
    glutAddMenuEntry("Cyan", opELLP|opOUTL|opCYN);
    glutAddMenuEntry("Magenta", opELLP|opOUTL|opMAG);
    glutAddMenuEntry("Yellow", opELLP|opOUTL|opYLW);
    glutAddMenuEntry("Black", opELLP|opOUTL|opBLK);
    glutAddMenuEntry("Brown", opELLP|opOUTL|opBWN);
  int ellp = glutCreateMenu(menuHandle);
    glutAddSubMenu ("Filled", ellp_filled);
    glutAddSubMenu ("Outline", ellp_outline);

  int line = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opLINE|opRED);
    glutAddMenuEntry("Green", opLINE|opGRN);
    glutAddMenuEntry("Blue", opLINE|opBLU);
    glutAddMenuEntry("Cyan", opLINE|opCYN);
    glutAddMenuEntry("Magenta", opLINE|opMAG);
    glutAddMenuEntry("Yellow", opLINE|opYLW);
    glutAddMenuEntry("Black", opLINE|opBLK);
    glutAddMenuEntry("Brown", opLINE|opBWN);

  int bezi = glutCreateMenu (menuHandle);
    glutAddMenuEntry("Red", opBEZI|opRED);
    glutAddMenuEntry("Green", opBEZI|opGRN);
    glutAddMenuEntry("Blue", opBEZI|opBLU);
    glutAddMenuEntry("Cyan", opBEZI|opCYN);
    glutAddMenuEntry("Magenta", opBEZI|opMAG);
    glutAddMenuEntry("Yellow", opBEZI|opYLW);
    glutAddMenuEntry("Black", opBEZI|opBLK);
    glutAddMenuEntry("Brown", opBEZI|opBWN);

  glutCreateMenu(menuHandle);
    glutAddSubMenu ("add Rectangle", rect);
    glutAddSubMenu ("add Ellipse", ellp);
    glutAddSubMenu ("add Line", line);
    glutAddSubMenu ("add Bezier Curve", bezi);

  glutAttachMenu (GLUT_LEFT_BUTTON);
}


/**Keypress function.
 * 'z' to undo
 */
void keyPressed(unsigned char key,int x, int y){
  switch (key)
  {
    // case 'z': shapeList.dequeue();
    // break;
    case 'z': shapeList.pop();
    break;
    default:
    break;
  }
  glutPostRedisplay();
}

void init(){
  glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0,WINDOW_WIDTH,WINDOW_HEIGHT,0); //top-left (0,0)
}

void display(){
  glClear (GL_COLOR_BUFFER_BIT);
  drawShape();
  glFlush();
}

void reshape(GLsizei width, GLsizei height) {

   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, width, height, 0); //top-left (0,0)
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();
}

int main(int argc, char *argv[]) {

  int window;
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (WINDOW_HEIGHT, WINDOW_WIDTH);
  glutInitWindowPosition (0, 0);
  window = glutCreateWindow ("Paint Program");

  init();
  menuInit();

  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutMouseFunc (mouse);
  glutKeyboardFunc (keyPressed);
  glutMainLoop ();

  return 0;
}

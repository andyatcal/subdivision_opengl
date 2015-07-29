// This is an implementatoin of Catmull Clark subdivision in OpenGL.
// Andy Wang June 22 2015

#include <GLUT/GLUT.h>
#include <vector>
#include <iostream>
#include <Eigen/Dense>

#define PI 3.14159265

using namespace std;
using namespace Eigen;

class Viewport;

class Viewport {
    public:
        int width, hight;
};

// Global Variables
Viewport viewport;
string USAGE = "Usage: catmull_clark_subdivision SUBDIVSION_LEVEL";
float angle = 0.0; //Initial Rotation Angle
bool wireframe = false; //Initial state of wireframe
bool smoothShading = false; //Initial state of smooth shading
bool lightOn = false; //Initial state of GL_light, off


// SET UP OF GL_LIHGTS -- Can be refined later
void setLight(){
    GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light1_diffuse[] = { 1.0, 0, 0, 1.0 };
    GLfloat light1_specular[] = { 1.0, 0, 0, 1.0 };
    GLfloat light1_position[] = { 100, 0, 0, 0.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

// Initiation
void init() {
    glClearColor(0, 0, 0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, viewport.width/viewport.hight, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    setLight();
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
    glLoadIdentity();
}

void draw2DPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x + 0.5, y + 0.5);
}

void draw3DPoint3f(Vector3f vertex) {
    glVertex3f(vertex[0], vertex[1], vertex[2]);
}

// DEFINITION of point
class AWPoint{
public:
    Vector3f position;
    bool exist;
    AWPoint(){exist = FALSE};
    AWPoint(float x, float y, float z){
        exist = TRUE;
        position = Vector3f(x, y, z);
    }
};

class AWPolygon{
public:
    vector<AWPoint> points; // The vertices of the face, should be larger than 3.
    AWPoint newFacePoint;
    AWPolygon(){
        points.clear();
    }
    void addPoint(AWPoint newPoint) {
        points.push_back(newPoint);
    }
    int size(){
        return points.size();
    }
    void computeNewFacePoint(){
        Vector3f result = Vector3f(0, 0, 0);
        for(vector<AWPoint>::iterator it = points.begin(); it != points.end(); it++) {
            AWPoint currentPoint = *it;
            result += currentPoint.position;
        }
        result /= size();
        newFacePoint.position = result;
    }
};

class AWEdge{
public:
    AWPolygon left;
    AWPolygon right;
    AWPoint start;
    AWPoint end;
    AWPoint newEdgePoint;
    void computeNewEdgePoint(){
        if(left.size() != 0 && right.size() != 0){
            Vector3f result = Vector3f(0, 0, 0);
            result += start.position;
            result += end.position;
            result += left.newFacePoint.position;
            result += right.newFacePoint.position;
            result /= 4;
            newEdgePoint.position = result;
        } else {
            newEdgePoint.exist = FALSE;
        }
    }
};

void testAWPolygon(){
    AWPoint p1 = AWPoint(1, 1, 1);
    AWPoint p2 = AWPoint(1, 1, -1);
    AWPoint p3 = AWPoint(1, -1, -1);
    AWPoint p4 = AWPoint(1, -1, 1);
    AWPoint p5 = AWPoint(-1, 1, 1);
    AWPoint p6 = AWPoint(-1, 1, -1);
    AWPoint p7 = AWPoint(-1, -1, -1);
    AWPoint p8 = AWPoint(-1, -1, 1);
    AWPolygon A;
    AWPolygon B;
    A.addPoint(p1);
    A.addPoint(p2);
    A.addPoint(p3);
    A.addPoint(p4);
    B.addPoint(p5);
    B.addPoint(p6);
    B.addPoint(p2);
    B.addPoint(p1);
    A.computeNewFacePoint();
    B.computeNewFacePoint();
    AWEdge e1;
    e1.left = A;
    e1.right = B;
    e1.start = p2;
    e1.end = p1;
    e1.computeNewEdgePoint();
    cout<<e1.newEdgePoint.position;
}
// A cube defined by its center and size.
class Cube{
public:
    float size;
    Vector3f center;

    // The verticies of the cube. "a means +, a_ means -"
    // Front face: abc, abc_, ab_c_, ab_c
    // Back face: a_bc, a_b_c, a_b_c_, a_bc_
    // Left face: ab_c, ab_c_, a_b_c_, a_b_c
    // Right face: abc, a_bc, a_bc_, abc_
    // Up face: a_b_c, a_bc, abc, ab_c
    // Down face: ab_c_, a_b_c_, a_bc_, abc_
    Vector3f abc;
    Vector3f abc_;
    Vector3f ab_c;
    Vector3f ab_c_;
    Vector3f a_bc;
    Vector3f a_bc_;
    Vector3f a_b_c;
    Vector3f a_b_c_;

    // Construct Cube
    Cube(float x, float y, float z, float size_input){
        center = Vector3f(x, y, z);
        size = size_input;
        updateCubeVertex();
    }

    void updateCubeVertex(){
        float halfSize = size/2;
        abc = center + Vector3f(halfSize, halfSize, halfSize);
        abc_ = center + Vector3f(halfSize, halfSize, -halfSize);
        ab_c = center + Vector3f(halfSize, -halfSize, halfSize);
        ab_c_ = center + Vector3f(halfSize, -halfSize, -halfSize);
        a_bc = center + Vector3f(-halfSize, halfSize, halfSize);
        a_bc_ = center + Vector3f(-halfSize, halfSize, -halfSize);
        a_b_c = center + Vector3f(-halfSize, -halfSize, halfSize);
        a_b_c_ = center + Vector3f(-halfSize, -halfSize, -halfSize);        
    }

    //Set the center of the cube
    void setCubeCenter(float x, float y, float z){
        center[0] = x;
        center[1] = y;
        center[2] = z;
        updateCubeVertex();
    }

    void resize(float size){
        size = size;
        updateCubeVertex();
    }
};

void drawCube(){
    // Front face: abc, abc_, ab_c_, ab_c
    // Back face: a_bc, a_b_c, a_b_c_, a_bc_
    // Left face: ab_c, ab_c_, a_b_c_, a_b_c
    // Right face: abc, a_bc, a_bc_, abc_
    // Top face: a_b_c, a_bc, abc, ab_c
    // Bottom face: ab_c_, a_b_c_, a_bc_, abc_
    Cube cube = Cube(0, 0, 0, 0.5);
    
    glBegin(GL_QUADS);
        //Front face
        glColor3f(1, 0, 0);
        draw3DPoint3f(cube.abc);
        draw3DPoint3f(cube.abc_);
        draw3DPoint3f(cube.ab_c_);
        draw3DPoint3f(cube.ab_c);
        //Back face
        glColor3f(0, 1, 0);
        draw3DPoint3f(cube.a_bc);
        draw3DPoint3f(cube.a_b_c);
        draw3DPoint3f(cube.a_b_c_);
        draw3DPoint3f(cube.a_bc_);
        //Left face
        glColor3f(0, 0, 1);
        draw3DPoint3f(cube.ab_c);
        draw3DPoint3f(cube.ab_c_);
        draw3DPoint3f(cube.a_b_c_);
        draw3DPoint3f(cube.a_b_c);
        //Right face
        glColor3f(1, 1, 0);
        draw3DPoint3f(cube.abc);
        draw3DPoint3f(cube.a_bc);
        draw3DPoint3f(cube.a_bc_);
        draw3DPoint3f(cube.abc_);
        //Top face
        glColor3f(1, 0, 1);
        draw3DPoint3f(cube.a_b_c);
        draw3DPoint3f(cube.a_bc);
        draw3DPoint3f(cube.abc);
        draw3DPoint3f(cube.ab_c);
        //Bottom face
        glColor3f(0, 1, 1);
        draw3DPoint3f(cube.ab_c_);
        draw3DPoint3f(cube.a_b_c_);
        draw3DPoint3f(cube.a_bc_);
        draw3DPoint3f(cube.abc_);
    glEnd();

}

void render() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5, 0, 0, 0, 0, 0, 0, 0, 1);	//	eye position, aim point, up direction
    //glPushMatrix ();
    //glTranslatef(0, 0, -4.0); //Linear Transformations
    //glScalef (scale, scale, scale);
    angle += 0.5;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 1, 1, 1);
    //glRotatef (zAngle, 0,0,1);
    //glRotatef (yAngle, 0,1,0);
    //glRotatef (xAngle, 1,0,0);
    drawCube();
    //glPopMatrix ();
	glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h==0) {h=1;} // prevent a division by zero when setting aspect ratio
    glViewport(0,0,w,h); // set the drawable region of the window
    glMatrixMode(GL_PROJECTION); // set up the projection matrix 
    glLoadIdentity(); 
    gluPerspective(45, (float)w/h, 0.1, 100); // just use a perspective projection
    glMatrixMode(GL_MODELVIEW); // go back to modelview matrix so we can move the objects about
    glLoadIdentity();
}

void myFrameMove() {
  glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {  
    if (key == 27) {exit(0);}
    if (key == 's') {
        if (smoothShading) {
            glShadeModel(GL_FLAT);
            smoothShading = false;
        } else {
            glShadeModel(GL_SMOOTH);
            smoothShading = true;
        }
    }
    if (key == 'w') {
        if (wireframe) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            wireframe = false;
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            wireframe = true;
        }
    }
    if (key == 'l') {
        if (lightOn) {
            glDisable(GL_LIGHTING);
            //glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            lightOn = false;
        } else {
            glEnable(GL_LIGHTING);
            //glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            lightOn = true;
        }
    }
    /*
    if (key == '+') {
        scale += 0.1;
    }
    if (key == '-') {
        scale -= 0.1;
    }
    if (key == GLUT_KEY_UP) {
        xAngle += 10;
    }
    if (key == GLUT_KEY_DOWN) {
        xAngle -= 10;
    }
    if (key == GLUT_KEY_RIGHT) {
        zAngle += 10;
    }
    if (key == GLUT_KEY_LEFT) {
        zAngle -= 10;
    }
    */
    glutPostRedisplay();
}  

void keySpecial(int key, int x, int y) {
    bool shiftDown = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
    /*
    if (shiftDown && (key == GLUT_KEY_LEFT)) {
        xTrans += 0.1;
    } else if (shiftDown && (key == GLUT_KEY_RIGHT)) {
        xTrans -= 0.1;
    } else if (shiftDown && (key == GLUT_KEY_UP)) {
        zTrans += 0.1;
    } else if (shiftDown && (key == GLUT_KEY_DOWN)) {
        zTrans -= 0.1;
    } else if (key == GLUT_KEY_LEFT) {
        zAngle -= 10;
    } else if (key == GLUT_KEY_RIGHT) {
        zAngle += 10;
    } else if (key == GLUT_KEY_UP) {
        xAngle += 10;
    } else if (key == GLUT_KEY_DOWN) {
        xAngle -= 10;
    }
    */
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    viewport.width = 640;
    viewport.hight = 480;
    glutInitWindowSize(viewport.width, viewport.hight);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Catmull Clark Subdivision");
    init();
    testAWPolygon();
    glutDisplayFunc(render);
    glutReshapeFunc(reshape);
    glutIdleFunc(myFrameMove); 
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(keySpecial);
    glutMainLoop();
    return 0;
}




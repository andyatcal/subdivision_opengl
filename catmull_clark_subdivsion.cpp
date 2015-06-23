// This is an implementatoin of Catmull Clark subdivision in OpenGL.
// Andy Wang June 22 2015

#include <GLUT/GLUT.h>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>

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

void Init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void set2DPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x + 0.5, y + 0.5);
}

class Vertex{
public:
    Vector3f position;
    Vector3f normal;
};


void render() {
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, 1);	//	eye position, aim point, up direction
    //glPushMatrix ();
    //glTranslatef(xTrans, yTrans, zTrans); //Linear Transformations
    //glScalef (scale, scale, scale);
    //glRotatef (zAngle, 0,0,1);
    //glRotatef (yAngle, 0,1,0);
    //glRotatef (xAngle, 1,0,0);
    glPopMatrix ();
	glutSwapBuffers();
}

void myReshape(int w, int h) {
    if (h==0) {h=1;} // prevent a division by zero when setting aspect ratio
    glViewport(0,0,w,h); // set the drawable region of the window
    glMatrixMode(GL_PROJECTION); // set up the projection matrix 
    glLoadIdentity(); 
    gluPerspective(45, (float)w/h, 0.1, 100); // just use a perspective projection
    glMatrixMode(GL_MODELVIEW); // go back to modelview matrix so we can move the objects about
    glLoadIdentity();
}

void keyPressed(unsigned char key, int x, int y) {  
    if (key == 27) {exit(0);}
    if (key == 's') {
        if (smooth) {
            glShadeModel(GL_FLAT);
            smooth = false;
        } else {
            glShadeModel(GL_SMOOTH);
            smooth = true;
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
    viewport.w = 640;
    viewport.h = 480;
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Catmull Clark Subdivision");
    Init();
    glutDisplayFunc(render);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(keySpecial);
    glutMainLoop();
    return 0;
}




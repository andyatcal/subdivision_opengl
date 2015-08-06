// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//


#include <GLUT/GLUT.h>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>
#include "vertex.h"
#include "halfedge.h"
#include "face.h"
#include "mesh.h"
#include "subdivision.h"
#include "meshUtils.h"
#include "viewport.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209

using namespace std;
using namespace glm;

//************************************************************
//          Global Variables
//************************************************************

// The viewport to diaplay graphics in OpenGL
Viewport viewport;
// USAGE MESSAGE
string USAGE = "USAGE: ./~ SUBDIVSION_LEVEL";
//Initial state of wireframe mode
bool wireframe = false;
//Initial state of smooth shading
bool smoothShading = false;
//Initial state of GL_Light
bool lightOn = false; 
//Initial Rotation Angle
float angle = 0.0; 
// Define a very small value
float VERYSMALLVALUE = 0.0001;
// The mesh to subdivide and display.
Mesh glMesh;

//************************************************************
//          Let's build some Sweeps!!
//************************************************************


void makeCircleSweep(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect) {
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
    while(!faceVect.empty()){
        tempFace = faceVect.back();
        faceVect.pop_back();
        delete tempFace;
    }
    while(!edgeVect.empty()){
        tempEdge = edgeVect.back();
        edgeVect.pop_back();
        delete tempEdge;
    }
    while(!vertVect.empty()){
        tempVert = vertVect.back();
        vertVect.pop_back();
        delete tempVert;
    }

    int loop_test = 0;
    int l_slices = 36;
    float total_arc = 360.0;
    float step = total_arc / l_slices;
    vector<vector<Vertex*> > vertices;

    for(float i = 0.0; i <= total_arc; i += step) {
        vector<Vertex*> crossSection;
        float angle = i / 180 * PI;
        vec3 zaxis = vec3(0, 0, 1);
        vec3 xaxis = vec3(1, 0, 0);
        vec3 v1 = vec3(0, 2, 1);
        vec3 v2 = vec3(0, 1, 2);
        vec3 v3 = vec3(0, -1, -2);
        vec3 v4 = vec3(0, -2, -1);
        vec3 trans = vec3(0 , 3, 0);
        /* //360
        v1 = rotate(v1, angle, xaxis);
        v2 = rotate(v2, angle, xaxis);
        v3 = rotate(v3, angle, xaxis);
        v4 = rotate(v4, angle, xaxis);
        */
        // I will twist m * 180 of this sweep.
        int m = 11;

        v1 = rotate(v1, m*angle/2, xaxis);
        v2 = rotate(v2, m*angle/2, xaxis);
        v3 = rotate(v3, m*angle/2, xaxis);
        v4 = rotate(v4, m*angle/2, xaxis);
        
        v1 += trans;
        v2 += trans;
        v3 += trans;
        v4 += trans;

        v1 = rotate(v1, angle, zaxis);
        v2 = rotate(v2, angle, zaxis);
        v3 = rotate(v3, angle, zaxis);
        v4 = rotate(v4, angle, zaxis);

        if(i == total_arc) {
            if(distance(v1, vertices[0][0] -> position) < VERYSMALLVALUE
             && distance(v4, vertices[0][3] -> position) < VERYSMALLVALUE) {
                loop_test = 1;
                cout<<"This is a normal loop."<<endl;
                break;
            } else if(distance(v1, vertices[0][3] -> position) < VERYSMALLVALUE
             && distance(v4, vertices[0][0] -> position) < VERYSMALLVALUE) {
                cout<<"This is a moibus loop."<<endl;
                loop_test = 2;
                break;
            }
        }

        Vertex * P1 = new Vertex;
        Vertex * P2 = new Vertex;
        Vertex * P3 = new Vertex;
        Vertex * P4 = new Vertex;

        P1 -> position = v1;
        P2 -> position = v2;
        P3 -> position = v3;
        P4 -> position = v4;

        vertVect.push_back(P1);
        vertVect.push_back(P2);
        vertVect.push_back(P3);
        vertVect.push_back(P4);

        crossSection.push_back(P1);
        crossSection.push_back(P2);
        crossSection.push_back(P3);
        crossSection.push_back(P4);
        vertices.push_back(crossSection);
    }
    for(size_t j = 0; j < vertices.size() - 1; j += 1) {
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][k];
            Vertex * v2 = vertices[j + 1][k];
            Vertex * v3 = vertices[j + 1][k + 1];
            Vertex * v4 = vertices[j][k + 1];
            makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
        }
    }
    if(loop_test == 1) { //If it is a loop //allignment test
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][k];
            Vertex * v2 = vertices[0][k];
            Vertex * v3 = vertices[0][k + 1];
            Vertex * v4 = vertices[j][k + 1];
            makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
        }        
    }
    if(loop_test == 2) { //If it is a mobius loop
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][3 - k - 1];
            Vertex * v2 = vertices[j][3 - k];
            Vertex * v3 = vertices[0][k];
            Vertex * v4 = vertices[0][k + 1];
            makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
        }        
    }
    //cout<<vertVect.size()<<" "<<faceVect.size()<<endl;
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    vector<Halfedge*>::iterator eIt;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 + 1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) && ((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            } else if (((*edgeIt1) -> start == (*edgeIt2) -> start) &&((*edgeIt1) -> end == (*edgeIt2) -> end)) {
                //cout<<"Hey, here is a mobius pair!"<<endl;
                //cout<<"One starts from vertex "<<(*edgeIt1) -> start -> ID<<" and it ends at vertex "<<(*edgeIt1) -> end -> ID<<endl;
                //cout<<"Another starts from vertex "<<(*edgeIt2) -> start -> ID<<" and it ends at vertex "<<(*edgeIt2) -> end -> ID<<endl;

                (*edgeIt1)->mobiusSibling = *edgeIt2;
                (*edgeIt2)->mobiusSibling = *edgeIt1;

                (*edgeIt1) -> start -> onMobiusSibling = true;
                (*edgeIt1) -> end -> onMobiusSibling = true;
                (*edgeIt2) -> start -> onMobiusSibling = true;
                (*edgeIt2) -> end -> onMobiusSibling = true;

            }
        }
    }
    vector<Halfedge*> boundaryEdges;
    for(eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++) {
        if((*eIt) -> sibling == NULL && (*eIt) -> mobiusSibling == NULL) {
            boundaryEdges.push_back(*eIt);
        }
    }
    for( edgeIt1 = boundaryEdges.begin(); edgeIt1 < boundaryEdges.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < boundaryEdges.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->start) &&((*edgeIt1)->end != (*edgeIt2)->end)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if (((*edgeIt1)->end == (*edgeIt2)->end) &&((*edgeIt1)->start != (*edgeIt2)->start)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if ((*edgeIt1)->start == (*edgeIt2)->end){

                (*edgeIt1)->previousBoundary = *edgeIt2;
                (*edgeIt2)->nextBoundary = *edgeIt1;

            } else if ((*edgeIt1)->end == (*edgeIt2)->start){

                (*edgeIt1)->nextBoundary = *edgeIt2;
                (*edgeIt2)->previousBoundary = *edgeIt1;

            }
        }
    }
    for(eIt = boundaryEdges.begin(); eIt < boundaryEdges.end(); eIt ++) {
        (*eIt) -> isSharp = true;
    }
}

// Initiate the mesh for OpenGL to render

void init(int level){
    makeCircleSweep(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //cout<<glMesh.FaceVect.size()<<" "<< glMesh.VertVect.size()<<endl;
    Subdivision myCC(glMesh);
    glMesh = myCC.ccSubdivision(level);
}
//************************************************************
//          OpenGL Display Functions
//************************************************************
void initRendering();

void render(void);

void reshape(int w, int h);

void keyboard(unsigned char c, int x, int y);

void keySpecial(int key, int x, int y);

void mousePressed(int button, int state, int x, int y);

void mouseMoved(int x, int y);

void initRendering(){

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient0[] = { 0.0, 0.0, 0.0, 10.0 };
    GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 10.0 };
    GLfloat light_specular0[] = { 1.0, 1.0, 1.0, 10.0 };
    GLfloat light_position0[] = { 0, 0, 1, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    glEnable(GL_LIGHT1);

    GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position1[] = { 0, 1, -0.5, 0.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHT2);

    GLfloat light_ambient2[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position2[] = { 0.877, -0.5, -0.5, 0.0 };

    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

    glEnable(GL_LIGHT3);

    GLfloat light_ambient3[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse3[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular3[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position3[] = { -0.877, -0.5, -0.5, 0.0 };

    glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse3);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular3);
    glLightfv(GL_LIGHT3, GL_POSITION, light_position3);

    GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat yello[] = {.8f, .8f, 0.f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, yello);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    GLfloat shininess[] = {50};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(20, 0, 0, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction
    vector<Face*>::iterator dispFaceIt;
    Face * tempFace;
    angle += 0.2;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 0, 0, 1);
    for(dispFaceIt = glMesh.FaceVect.begin(); dispFaceIt < glMesh.FaceVect.end(); dispFaceIt++){
        tempFace = *dispFaceIt;
        Vertex * tempv;
        vector<Vertex*>::iterator vIt;
        vector<Vertex*> vertices = tempFace -> vertices;
        glBegin(GL_QUADS);
        for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
            tempv = *vIt;
            float normx = tempv -> position[0];
            float normy = tempv -> position[1];
            float normz = tempv -> position[2];
            glNormal3f(normx, normy, normz);
            float x = tempv -> position[0];
            float y = tempv -> position[1];
            float z = tempv -> position[2];
            //cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h==0) {h=1;} // prevent a division by zero when setting aspect ratio
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    gluPerspective(45, (float)w/h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myFrameMove() {
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
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
            lightOn = false;
        } else {
            glEnable(GL_LIGHTING);
            lightOn = true;
        }
    }
    glutPostRedisplay();
}

void keySpecial(int key, int x, int y) {
    bool shiftDown = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
    glutPostRedisplay();
}

void mousePressed(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        exit(0);
    }
}

void mouseMoved(int x, int y){
    
}

//************************************************************
//          Main Function
//************************************************************

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    viewport.width = 640;
    viewport.hight = 480;

    //Process the command line arguments
    int level;
    string inputSIF;
    if(argc <= 1 || argc > 2){
        cout<<USAGE;
        exit(1);
    } else if(argc == 2){
        level = stoi(argv[1]);
        init(level);
    }
    glutInitWindowSize(viewport.width, viewport.hight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    initRendering();
    glutDisplayFunc(render);
    // General UI functions
    glutReshapeFunc(reshape);
    glutIdleFunc(myFrameMove); 
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keySpecial);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mouseMoved);

    glutMainLoop();
    
}

// Catmull-Clark Subdivision Project
// With sharp edge feature.
// Andy Wang, UC Berkeley July 1st 2015

#include <GLUT/GLUT.h>
#include <math.h>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#define PI 3.1415926535897932384626433832795028841971693993751058209

using namespace std;
using namespace Eigen;

//************************************************************
//          Viewport class
//************************************************************
class Viewport;

class Viewport {
    public:
        int width, hight;
};

//************************************************************
//          Global Variables
//************************************************************

Viewport viewport;
string USAGE = "USAGE: cc SUBDIVSION_LEVEL";
//float angle = 0.0; //Initial Rotation Angle
bool wireframe = false; //Initial state of wireframe
bool smoothShading = false; //Initial state of smooth shading
bool lightOn = false; //Initial state of GL_Light
float angle = 0.0; //Initial Rotation Angle
bool checkCC = true;

//************************************************************
//          Geometry Classes
//************************************************************

// Mesh class. A Mesh class contains a list of faces, a list of
// vertices, and a list of edges. It also specifies the connectivity
// among those geometries.
class Mesh;

// Face class. A face represents a list of vertices (more than 3)
// connected by edges. Initially, set as a quad.
class Face;

// Halfedge class. A halfedge represents an edge starting from one vertex
// and pointing to another vertex.
class Halfedge;

// Vertex class. A vertex contains a point in the 3D space. With
// one out-going halfedge
class Vertex;

class Vertex{
public:
    Vertex();
    Vertex(float ix, float iy, float iz);
    Vertex(Vector3f iposition);
    Vector3f position;
    Halfedge * oneOutEdge;
    Vector3f normal;
};

Vertex::Vertex(){
    position = Vector3f(0, 0, 0);
    oneOutEdge = NULL;
}

Vertex::Vertex(float ix, float iy, float iz) {
    position = Vector3f(ix, iy, iz);
    oneOutEdge = NULL;
}

Vertex::Vertex(Vector3f iposition) {
    position = iposition;
    oneOutEdge = NULL;
}

class Face{
public:
    Face();
    vector<Vertex*> vertices;
    Halfedge * oneSideEdge;
    Vertex * facePoint;
    void addVertex(Vertex * v);
};

Face::Face(){
    oneSideEdge = NULL;
    facePoint = NULL;
    vertices.clear();
}

void Face::addVertex(Vertex * v){
    vertices.push_back(v);
}

class Halfedge{
public:
    Halfedge();
    Vertex * start;
    Vertex * end;
    Halfedge * sibling;
    Halfedge * next;
    Halfedge * previous;
    Face * heFace;
    // Do we need a pointer to the sibling halfedge?

    Vertex * edgePoint;
    Halfedge * firstHalf;
    Halfedge * secondHalf;
    bool isSharp;
};

Halfedge::Halfedge(){
    start = end = NULL;
    sibling = NULL;
    next = previous = NULL;
    heFace = NULL;
    edgePoint = NULL;
    firstHalf = secondHalf = NULL;
    isSharp = false; // On the border
}

class Mesh{
public:
    Mesh();
    vector<Vertex*> VertVect;
    vector<Halfedge*> EdgeVect;
    vector<Face*> FaceVect;
    Mesh duplicate(Mesh original);
};

Mesh::Mesh(){}
Mesh Mesh::duplicate(Mesh original){
    Mesh newMesh = Mesh();
    // Need to be modified
    return newMesh;
}

//global variable for mesh
Mesh glMesh;

//************************************************************
//          Subdivision Functions
//************************************************************

// Construct facepoints
void makeFacePoints(vector<Face*> &faceVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator it;
    for(it = faceVect.begin(); it < faceVect.end(); it++){
        vector<Vertex*> vertices = (*it) -> vertices;
        // Facepoint is the average of vertices in this face
        Vertex * newFacePoint = new Vertex;
        Vector3f newFacePointPosition = Vector3f(0, 0, 0);
        vector<Vertex*>::iterator vIt;
        if(!vertices.empty()) {
            for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
                newFacePointPosition += ((*vIt) -> position);
            }
            newFacePointPosition /= vertices.size();
            newFacePoint -> position = newFacePointPosition;
        } else {
            cout << "ERROR: CAN NOT CALCULATE FACEPOINTS WITH LESS THAN 3 VERTICES."<<endl;
            exit(0);
        }
        (*it) -> facePoint = newFacePoint;
        vertVect.push_back(newFacePoint);
    }
}


// Construct edge points
void makeEdgePoints(vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Halfedge*>::iterator it;
    for(it = edgeVect.begin(); it < edgeVect.end(); it++){
        if((*it) -> sibling -> edgePoint != NULL){
            (*it) -> edgePoint = (*it) -> sibling -> edgePoint;
        } else {
            Vertex faceVert1 = *((*it) -> heFace -> facePoint);
            Vertex faceVert2 = *((*it) -> sibling -> heFace -> facePoint);
            Vertex edgeVert1 = *((*it) -> end);
            Vertex edgeVert2 = *((*it) -> sibling -> end);

            Vertex * newEdgePoint = new Vertex;
            (*newEdgePoint).position = (faceVert1.position + faceVert2.position + edgeVert1.position + edgeVert2.position)/4;
            (*it) -> edgePoint = newEdgePoint;

            vertVect.push_back(newEdgePoint);
        }
        
    }
}

void makeVertexPoints(vector<Vertex*> &vertVect){
    vector<Vertex*>::iterator it;
    Vertex facePointAvg;
    Vertex edgePointAvg;
    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;
        Halfedge * firstOutEdge;
        firstOutEdge = currVert -> oneOutEdge;
        Halfedge * nextOutEdge = firstOutEdge;
        Vector3f newFacePointAvgPosition = Vector3f(0, 0, 0);
        Vector3f newEdgePointAvgPoistion = Vector3f(0, 0, 0);
        int n = 0;
        do {
            newEdgePointAvgPoistion += nextOutEdge -> edgePoint -> position;
            newFacePointAvgPosition += nextOutEdge -> heFace -> facePoint -> position;
            n += 1;
            nextOutEdge = nextOutEdge -> sibling -> next;
        } while ( nextOutEdge != firstOutEdge); // Need to check if we can go back to the first when there are borders;
        newFacePointAvgPosition /= n;
        newEdgePointAvgPoistion /= n;
        edgePointAvg.position = newEdgePointAvgPoistion;
        facePointAvg.position = newFacePointAvgPosition;
        // A special case when n == 3.
        if(n == 3) { n = 4;}
        currVert->position = ((n - 3)*currVert->position +  2*edgePointAvg.position + facePointAvg.position)/n; 
    }
}

// Create conections between the new points
void compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect, vector<Halfedge*> &newEdgeVect){
    vector<Face*>::iterator it;
    for(it = faceVect.begin(); it < faceVect.end(); it++){
        Face currFace = **it;
        Halfedge * firstEdge = currFace.oneSideEdge;
        Halfedge * currEdge = currFace.oneSideEdge;
        Halfedge * previousB = new Halfedge;
        Halfedge * previousOut = new Halfedge;
        Halfedge * edgeA;
        Halfedge * edgeB;
        Halfedge * edgeIn;
        Halfedge * edgeOut;     
        Face * newFace;   
        // Split the edges and create "in and out" edges.`
        do{
            edgeA = new Halfedge;
            edgeB = new Halfedge;
            edgeIn = new Halfedge;
            edgeOut = new Halfedge;
            currEdge -> firstHalf = edgeA;
            currEdge -> secondHalf = edgeB;
            edgeA -> start = currEdge -> start;
            edgeA -> end = currEdge -> edgePoint;
            edgeB -> start = currEdge -> edgePoint;
            edgeB -> end = currEdge -> end;
            if(currEdge -> sibling -> firstHalf != NULL) {
                edgeB -> sibling = currEdge -> sibling -> firstHalf;
                edgeA -> sibling = currEdge -> sibling -> secondHalf;

                currEdge -> sibling -> firstHalf -> sibling = edgeB;
                currEdge -> sibling -> secondHalf -> sibling = edgeA;
            }
            currEdge -> start -> oneOutEdge = edgeA;
            edgeIn -> start = currEdge -> edgePoint;
            edgeIn -> end = currFace.facePoint;
            edgeOut -> end = currEdge -> edgePoint;
            edgeOut -> start = currFace.facePoint;
            edgeIn->sibling = edgeOut;
            edgeOut->sibling = edgeIn;
            currEdge -> edgePoint -> oneOutEdge = edgeIn;
            currFace.facePoint -> oneOutEdge = edgeOut;
            edgeA -> next = edgeIn;
            edgeIn -> next = previousOut;
            previousOut -> next = previousB;
            previousB -> next = edgeA;
            newFace = new Face;
            newFace -> addVertex(edgeA -> start);
            newFace -> addVertex(edgeIn -> start);
            newFace -> addVertex(previousOut -> start);
            newFace -> addVertex(previousB -> start);
            edgeA -> heFace = newFace;
            edgeIn -> heFace = newFace;
            previousOut -> heFace = newFace;
            previousB -> heFace = newFace;
            newFace -> oneSideEdge = previousB;

            newEdgeVect.push_back(edgeA);
            newEdgeVect.push_back(edgeB);
            newEdgeVect.push_back(edgeIn);
            newEdgeVect.push_back(edgeOut);
            if(currEdge!= firstEdge){
                newFaceVect.push_back(newFace);
            }
            currEdge = currEdge -> next;
            previousB = edgeB;
            previousOut = edgeOut;
        } while (currEdge != currFace.oneSideEdge);

        //Connect the first and last
        edgeA = currFace.oneSideEdge -> firstHalf;
        edgeIn = edgeA -> next;
        edgeIn -> next = previousOut;
        previousOut -> next = previousB;
        previousB -> next = edgeA;
        newFace = new Face;
        newFace -> addVertex(edgeA -> start);
        newFace -> addVertex(edgeIn -> start);
        newFace -> addVertex(previousOut -> start);
        newFace -> addVertex(previousB -> start);
        edgeA -> heFace = newFace;
        edgeIn -> heFace = newFace;
        previousOut -> heFace = newFace;
        previousB -> heFace = newFace;
        newFace -> oneSideEdge = previousB;
        newFaceVect.push_back(newFace);
    }
}

//compute the normal vector of the vertex at the end of the input half edge
Vector3f getNormal(Halfedge * currEdge){
    Vertex * v1 = currEdge -> start;
    Vertex * v2 = currEdge -> end;
    Vertex * v3 = currEdge -> next -> end;

    Vector3f outNorm;

    Vector3f oneEdge = v2 -> position - v1 -> position;
    Vector3f secondEdge = v3 -> position - v2 -> position;

    return oneEdge.cross(secondEdge);
}

//iterate over every vertex in the mesh and compute its normal
void computeNormals(vector<Vertex*> &vertVect){

    vector<Vertex*>::iterator it;
    Halfedge * firstOutEdge;
    Halfedge * nextOutEdge;
    Vector3f avgNorm;
    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;
        int n = 0;
        firstOutEdge = currVert -> oneOutEdge;
        nextOutEdge = firstOutEdge;
        do {
            avgNorm += getNormal(nextOutEdge -> sibling);
            n += 1;
            nextOutEdge = nextOutEdge -> sibling -> next;
        } while ( nextOutEdge != firstOutEdge); // Need to check if we can go back to the first when there are borders;
        avgNorm /= n;
        currVert->normal = avgNorm;
    }
}


void ccSubDivision(){
    Mesh mesh;
    Halfedge * tempEdge;
    Face * tempFace;

    mesh.VertVect = glMesh.VertVect;

    makeFacePoints(glMesh.FaceVect, glMesh.VertVect);

    makeEdgePoints(glMesh.EdgeVect, glMesh.VertVect);

    makeVertexPoints(mesh.VertVect);

    compileNewMesh(glMesh.FaceVect, mesh.FaceVect, mesh.EdgeVect);

    computeNormals(glMesh.VertVect);

    while(!glMesh.FaceVect.empty()){
        tempFace = glMesh.FaceVect.back();
        glMesh.FaceVect.pop_back();
        delete tempFace;
    }

    while(!glMesh.EdgeVect.empty()){
        tempEdge = glMesh.EdgeVect.back();
        glMesh.EdgeVect.pop_back();
        delete tempEdge;
    }

    glMesh.FaceVect = mesh.FaceVect;
    glMesh.EdgeVect = mesh.EdgeVect;
}


//************************************************************
//          Let's build a Cube!!
//************************************************************
void makeTriFace(Vertex * va, Vertex* vb, Vertex * vc, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e1->end = vb;
    e2->end = vc;
    e3->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;

    e1->next = e2;
    e2->next = e3;
    e3->next = e1;
    
    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;

    nextFace->oneSideEdge = e3;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
}

void makeRectFace(Vertex * va, Vertex* vb, Vertex * vc, Vertex * vd, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3, *e4;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;
    e4 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);
    nextFace -> addVertex(vd);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e4->start = vd;
    e1->end = vb;
    e2->end = vc;
    e3->end = vd;
    e4->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;
    vd->oneOutEdge = e4;

    e1->next = e2;
    e2->next = e3;
    e3->next = e4;
    e4->next = e1;
    
    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;
    e4->heFace = nextFace;

    nextFace->oneSideEdge = e4;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
    edgeVect.push_back(e4);
}

void makePolygonFace(vector<Vertex*> vertices, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    vector<Halfedge*> edges;
    vector<Vertex*>::iterator vIt;
    Vertex * currVert;
    Halfedge * currEdge;
    for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
        currVert = *vIt;
        Halfedge * tempEdge = new Halfedge;
        tempEdge -> start = *vIt;
        if(vIt == (vertices.end() - 1)) {
            tempEdge -> end = *(vertices.begin());
        } else {
            tempEdge -> end = *(vIt + 1);
        }
        edges.push_back(tempEdge);
        currVert -> oneOutEdge = tempEdge;
        nextFace -> addVertex(*vIt);
    }
    
    vector<Halfedge*>::iterator eIt;
    for(eIt = edges.begin(); eIt < edges.end(); eIt++) {
        currEdge = *eIt;
        if(eIt == (edges.end() - 1)) {
            currEdge -> next = *(edges.begin());
        } else {
            currEdge -> next = *(eIt + 1);
        }
        currEdge -> heFace = nextFace;
        edgeVect.push_back(*eIt);
    }
    nextFace -> oneSideEdge = *(edges.begin());
    faceVect.push_back(nextFace);
}

void makePyramid(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
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

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
 
    v1 -> position = Vector3f(0, 0, 1);
    v2 -> position = Vector3f(1, 1, -1);
    v3 -> position = Vector3f(1, -1, -1);
    v4 -> position = Vector3f(-1, -1, -1);
    v5 -> position = Vector3f(-1, 1, -1);

    vector<Vertex*> bottomFace;
    bottomFace.push_back(v3);
    bottomFace.push_back(v2);
    bottomFace.push_back(v5);
    bottomFace.push_back(v4);
    //bottomFace
    makePolygonFace(bottomFace, faceVect, edgeVect);
    //cout<<faceVect[0];
    //makeRectFace(v3, v2, v5, v4, faceVect, edgeVect);
    //topfrontFace
    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    //topbackFace
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    //topleftFace
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    //toprightFace
    makeTriFace(v1, v5, v2, faceVect, edgeVect);

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
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

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = Vector3f(1, 1, 1);
    v2 -> position = Vector3f(1, -1, 1);
    v3 -> position = Vector3f(-1, -1, 1);
    v4 -> position = Vector3f(-1, 1, 1);
    v5 -> position = Vector3f(1, 1, -1);
    v6 -> position = Vector3f(1, -1, -1);
    v7 -> position = Vector3f(-1, -1, -1);
    v8 -> position = Vector3f(-1, 1, -1);

    //topFace
    makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){
                //cout<<"1";
                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

//************************************************************
//          OpenGL Display Functions
//************************************************************
void init(int level);

void render(void);

void reshape(int w, int h);

void keyboard(unsigned char c, int x, int y);

void mouse(int button, int state, int x, int y);

void init(int level){
    //makeCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    makePyramid(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    makeCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //cout<< glMesh.FaceVect.size()<<" "<<glMesh.EdgeVect.size()<<" "<<glMesh.VertVect.size();
    //computeNormals(glMesh.VertVect);
    ccSubDivision();
    //for(int i = 0; i < level; i++) {
    //    ccSubDivision();
    //}
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5, 0, 0, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction

    vector<Face*>::iterator dispFaceIt;
    Face * tempFace;
    angle += 0.1;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 0, 0, 1);
    for(dispFaceIt = glMesh.FaceVect.begin(); dispFaceIt < glMesh.FaceVect.end(); dispFaceIt++){
        tempFace = *dispFaceIt;
        Vertex * tempv;
        vector<Vertex*>::iterator vIt;
        vector<Vertex*> vertices = tempFace -> vertices;
        glBegin(GL_POLYGON);
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

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        exit(0);
    }
}

//************************************************************
//          Main Function
//************************************************************

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    viewport.width = 640;
    viewport.hight = 480;
    //int level = stoi(argv[1]);
    //init(level);
    glutInitWindowSize(viewport.width, viewport.hight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(render);
    // General UI functions
    glutReshapeFunc(reshape);
    glutIdleFunc(myFrameMove); 
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keySpecial);

    glutMainLoop();
}


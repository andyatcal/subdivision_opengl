// Catmull-Clark Subdivision Project
// With sharp edge feature.
// With Mobius edge feature.
// Andy Wang, UC Berkeley July 1st 2015

#include <GLUT/GLUT.h>
#include <math.h>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

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
    int ID;
    bool onMobiusSibling;

    Vector3f copy; // This is a copy of position, to serve makeVertexPoint function.
    void makeCopy();
};

Vertex::Vertex(){
    position = Vector3f(0, 0, 0);
    oneOutEdge = NULL;
    ID = 0;
    onMobiusSibling = false;
}

Vertex::Vertex(float ix, float iy, float iz) {
    position = Vector3f(ix, iy, iz);
    oneOutEdge = NULL;
    ID = 0;
    onMobiusSibling = false;
}

Vertex::Vertex(Vector3f iposition) {
    position = iposition;
    oneOutEdge = NULL;
    ID = 0;
    onMobiusSibling = false;
}

void Vertex::makeCopy() {
    copy = position;
}

class Face{
public:
    Face();
    vector<Vertex*> vertices;
    Halfedge * oneSideEdge;
    Vertex * facePoint;
    void addVertex(Vertex * v);
    int ID;
};

Face::Face(){
    oneSideEdge = NULL;
    facePoint = NULL;
    vertices.clear();
    ID = 0;
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

    // Sharp and Boundary features
    bool isSharp;
    Halfedge * nextBoundary;
    Halfedge * previousBoundary;

    //Mobius features
    Halfedge * mobiusSibling;
    Halfedge * mobiusBoundary;

    int ID;
};

Halfedge::Halfedge(){
    start = end = NULL;
    sibling = NULL;
    next = previous = NULL;
    heFace = NULL;
    edgePoint = NULL;
    firstHalf = secondHalf = NULL;
    isSharp = false; // On the border
    nextBoundary = NULL;
    previousBoundary = NULL;
    mobiusSibling = NULL;
    mobiusBoundary = NULL;
    ID = 0;
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
            //cout<<newFacePointPosition<<endl;
        } else {
            cout << "ERROR: CAN NOT CALCULATE FACEPOINTS WITH LESS THAN 0 VERTICES."<<endl;
            exit(0);
        }
        (*it) -> facePoint = newFacePoint;
        vertVect.push_back(newFacePoint);
        //cout<<"FacePoint: "<<endl;
        //cout<<newFacePointPosition;
    }
}


// Construct edge points
void makeEdgePoints(vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Halfedge*>::iterator it;
    for(it = edgeVect.begin(); it < edgeVect.end(); it++){
        if((*it) -> isSharp) {
            if((*it) -> sibling != NULL && (*it) -> sibling -> edgePoint != NULL){
                (*it) -> edgePoint = (*it) -> sibling -> edgePoint;
            } else if((*it) -> mobiusSibling != NULL && (*it) -> mobiusSibling -> edgePoint != NULL){
                (*it) -> edgePoint = (*it) -> mobiusSibling -> edgePoint;
            } else {
                Vertex * newEdgePoint = new Vertex;
                (*newEdgePoint).position = ((*it) -> end -> position + (*it) -> start -> position) / 2;
                //cout<<"sharp edge starting from vertex"<<(*it) -> start -> ID<<endl;
                //cout<<(*newEdgePoint).position<<endl;
                (*it) -> edgePoint = newEdgePoint;
                vertVect.push_back(newEdgePoint);
            }
        } else {
            if((*it) -> sibling != NULL && (*it) -> sibling -> edgePoint != NULL){
                (*it) -> edgePoint = (*it) -> sibling -> edgePoint;
            } else if((*it) -> sibling != NULL){
                Vertex faceVert1 = *((*it) -> heFace -> facePoint);
                Vertex faceVert2 = *((*it) -> sibling -> heFace -> facePoint);
                Vertex edgeVert1 = *((*it) -> end);
                Vertex edgeVert2 = *((*it) -> start);
                Vertex * newEdgePoint = new Vertex;
                (*newEdgePoint).position = (faceVert1.position + faceVert2.position + edgeVert1.position + edgeVert2.position) / 4;
                //cout<<"normal edge starting from vertex"<<(*it) -> start -> ID<<endl;
                //cout<<(*newEdgePoint).position<<endl;
                (*it) -> edgePoint = newEdgePoint;
                vertVect.push_back(newEdgePoint);
            } else if((*it) -> mobiusSibling != NULL && (*it) -> mobiusSibling -> edgePoint != NULL) {
                (*it) -> edgePoint = (*it) -> mobiusSibling -> edgePoint;
            } else {
                Vertex faceVert1 = *((*it) -> heFace -> facePoint);
                Vertex faceVert2 = *((*it) -> mobiusSibling -> heFace -> facePoint);
                Vertex edgeVert1 = *((*it) -> end);
                Vertex edgeVert2 = *((*it) -> start);
                Vertex * newEdgePoint = new Vertex;
                (*newEdgePoint).position = (faceVert1.position + faceVert2.position + edgeVert1.position + edgeVert2.position) / 4;
                (*newEdgePoint).onMobiusSibling = true;
                //cout<<"mobius edge starting from vertex"<<(*it) -> start -> ID<<endl;
                //cout<<(*newEdgePoint).position<<endl;
                (*it) -> edgePoint = newEdgePoint;
                vertVect.push_back(newEdgePoint);
                //cout<<"HEY!"<<endl;
            }   
        }
    }
}

void makeVertexPoints(vector<Vertex*> &vertVect){
    vector<Vertex*>::iterator it;
    Vertex facePointAvg;
    Vertex edgePointAvg;
    Vertex * currVert;
    //make a copy of the original mesh vertex points.
    for(it = vertVect.begin(); it < vertVect.end(); it++){
        (*it) -> makeCopy();
    }

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;
        //cout<<"vertexID: "<<currVert -> ID<<endl;
        Halfedge * firstOutEdge;
        firstOutEdge = currVert -> oneOutEdge;
        Halfedge * nextOutEdge = firstOutEdge;

        int sharpEdgeCounter = 0;
        int mobiusCounter = 0;

        Halfedge * sharpEdgeI;
        Halfedge * sharpEdgeK;

        Vector3f newFacePointAvgPosition = Vector3f(0, 0, 0);
        Vector3f newEdgePointAvgPoistion = Vector3f(0, 0, 0);
        int n = 0;
        do {
            //cout<<"here"<<endl<<nextOutEdge -> end -> ID<<endl;
            newEdgePointAvgPoistion += nextOutEdge -> edgePoint -> position;
            newFacePointAvgPosition += nextOutEdge -> heFace -> facePoint -> position;
            n += 1;
            if(nextOutEdge -> isSharp) {
                //cout<<"A"<<endl;
                sharpEdgeCounter += 1;
                if(sharpEdgeCounter == 1) {
                    sharpEdgeI = nextOutEdge;
                } else if(sharpEdgeCounter == 2) {
                    sharpEdgeK = nextOutEdge;
                }
                if(nextOutEdge -> sibling == NULL && nextOutEdge -> mobiusSibling == NULL) { // It is on a boundary!
                    //cout<<"A1"<<endl;
                    sharpEdgeCounter += 1;
                    if(!(currVert -> onMobiusSibling)) { // It is on a normal boundary!
                        //cout<<"A11"<<endl;
                        newEdgePointAvgPoistion += nextOutEdge -> previousBoundary -> edgePoint -> position;
                        if(sharpEdgeCounter == 1) {
                            sharpEdgeI = nextOutEdge -> previousBoundary;
                        } else if(sharpEdgeCounter == 2) {
                            sharpEdgeK = nextOutEdge -> previousBoundary;
                        }
                        if(mobiusCounter % 2 == 0) {
                            nextOutEdge = nextOutEdge -> previousBoundary -> next;
                        } else {
                            nextOutEdge = nextOutEdge -> previousBoundary -> previous;
                        }
                    } else { //It is on a mobius boundary.
                        //cout<<"A12"<<endl;
                        mobiusCounter += 1;

                        newEdgePointAvgPoistion += nextOutEdge -> mobiusBoundary -> edgePoint -> position;

                        if(sharpEdgeCounter == 1) {
                            sharpEdgeI = nextOutEdge -> mobiusBoundary;
                        } else if(sharpEdgeCounter == 2) {
                            sharpEdgeK = nextOutEdge -> mobiusBoundary;
                        }
                        if(mobiusCounter % 2 == 0) {
                            nextOutEdge = nextOutEdge -> mobiusBoundary -> next;
                        } else {
                            //bool ak = nextOutEdge -> mobiusBoundary -> previous == NULL;
                            //cout<<ak<<endl;
                            nextOutEdge = nextOutEdge -> mobiusBoundary -> previous;
                            //cout<<nextOutEdge -> start -> ID<<endl;
                        }
                    }
                } else if(nextOutEdge -> sibling != NULL){ // It has a normal sibling
                    //cout<<"A2"<<endl;
                    if(mobiusCounter % 2 == 0) {
                        nextOutEdge = nextOutEdge -> sibling -> next;
                    } else {
                        nextOutEdge = nextOutEdge -> sibling -> previous;
                    }
                } else { //when "nextOutEdge -> mobiusSibling != NULL"
                    //cout<<"A3"<<endl;
                    mobiusCounter += 1;
                    if(mobiusCounter % 2 == 0) {
                        nextOutEdge = nextOutEdge -> mobiusSibling -> next;
                    } else {
                        nextOutEdge = nextOutEdge -> mobiusSibling -> previous;
                    }
                }
            } else { // When it is not sharp.
                //cout<<"B"<<endl;
                if(nextOutEdge -> sibling != NULL) {
                    //cout<<"B1"<<endl;
                    if(mobiusCounter % 2 == 0) {
                        nextOutEdge = nextOutEdge -> sibling -> next;
                    } else {
                        nextOutEdge = nextOutEdge -> sibling -> previous;
                    }
                } else if(nextOutEdge -> mobiusSibling != NULL){
                    //cout<<"B2"<<endl;
                    mobiusCounter += 1;
                    if(mobiusCounter % 2 == 0) {
                        nextOutEdge = nextOutEdge -> mobiusSibling -> next;
                    } else {
                        nextOutEdge = nextOutEdge -> mobiusSibling -> previous;
                    }
                }
            }
        } while ( nextOutEdge != firstOutEdge); // Need to check if we can go back to the first when there are borders;
        if(sharpEdgeCounter <= 1) {
            newFacePointAvgPosition /= n;
            newEdgePointAvgPoistion /= n;
            edgePointAvg.position = newEdgePointAvgPoistion;
            facePointAvg.position = newFacePointAvgPosition;
            currVert->position = ((n - 2) * currVert->position +  edgePointAvg.position + facePointAvg.position) / n;
            //cout<<"this is a normal vertex!"<<currVert -> position<<endl;            
        } else if(sharpEdgeCounter == 2) {
            Vertex * pointI;
            if(sharpEdgeI -> end == currVert) {
                pointI = sharpEdgeI -> start;
            } else {
                pointI = sharpEdgeI -> end;
            }
            Vertex * pointK;
            if(sharpEdgeK -> end == currVert) {
                pointK = sharpEdgeK -> start;
            } else {
                pointK = sharpEdgeK -> end;
            }
            currVert -> position = (pointI -> copy + pointK -> copy + 6 * currVert -> copy) / 8;
            //cout<<"this is a crease vertex!"<<currVert -> position<<endl;
        } else {
            currVert -> position = currVert -> position; // Nothing happens when sharp edges is more than 3.
            //cout<<"this is a conner vertex!"<<currVert -> position<<endl;
        }
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
            if(currEdge -> isSharp) {
                edgeA -> isSharp = true;
                edgeB -> isSharp = true;
                // For boundary
                if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {//It is on boundary!
                    edgeB -> previousBoundary = edgeA;
                    edgeA -> nextBoundary = edgeB;
                    if(currEdge -> previousBoundary != NULL && currEdge -> previousBoundary -> secondHalf != NULL) {
                        currEdge -> previousBoundary -> secondHalf -> nextBoundary = edgeA;
                        edgeA -> previousBoundary = currEdge -> previousBoundary -> secondHalf;
                    }
                    if(currEdge -> nextBoundary != NULL && currEdge -> nextBoundary -> firstHalf != NULL) {
                        currEdge -> nextBoundary -> firstHalf -> previousBoundary = edgeB;
                        edgeB -> nextBoundary = currEdge -> nextBoundary -> firstHalf;
                    }
                    if(currEdge -> mobiusBoundary != NULL && currEdge -> mobiusBoundary -> firstHalf != NULL) {
                        if(currEdge -> start == currEdge -> mobiusBoundary -> start) { // They share the same start.
                            edgeA -> mobiusBoundary = currEdge -> mobiusBoundary -> firstHalf;
                            currEdge -> mobiusBoundary -> firstHalf -> mobiusBoundary = edgeA;
                        } else { // They share the same end.
                            edgeB -> mobiusBoundary = currEdge -> mobiusBoundary -> secondHalf;
                            currEdge -> mobiusBoundary -> secondHalf -> mobiusBoundary = edgeB;
                        }
                    }
                }
            }
            if(currEdge -> sibling != NULL && currEdge -> sibling -> firstHalf != NULL) {
                edgeB -> sibling = currEdge -> sibling -> firstHalf;
                edgeA -> sibling = currEdge -> sibling -> secondHalf;

                currEdge -> sibling -> firstHalf -> sibling = edgeB;
                currEdge -> sibling -> secondHalf -> sibling = edgeA;
            } else if(currEdge -> mobiusSibling != NULL && currEdge -> mobiusSibling -> firstHalf != NULL) {
                edgeA -> mobiusSibling = currEdge -> mobiusSibling -> firstHalf;
                edgeB -> mobiusSibling = currEdge -> mobiusSibling -> secondHalf;

                currEdge -> mobiusSibling -> firstHalf -> mobiusSibling = edgeA;
                currEdge -> mobiusSibling -> secondHalf -> mobiusSibling = edgeB;                
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
            edgeIn -> previous = edgeA;
            edgeIn -> next = previousOut;
            previousOut -> previous = edgeIn;
            previousOut -> next = previousB;
            previousB -> previous = previousOut;
            previousB -> next = edgeA;
            edgeA -> previous = previousB;
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
        edgeA -> next = edgeIn;
        edgeIn -> previous = edgeA;
        edgeIn -> next = previousOut;
        previousOut -> previous = edgeIn;
        previousOut -> next = previousB;
        previousB -> previous = previousOut;
        previousB -> next = edgeA;
        edgeA -> previous = previousB;
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

    oneEdge.normalize();
    secondEdge.normalize();

    Vector3f result = oneEdge.cross(secondEdge);
    result.normalize();

    return result;
}

//iterate over every vertex in the mesh and compute its normal
void computeNormals(vector<Vertex*> &vertVect){

    vector<Vertex*>::iterator it;
    Halfedge * firstOutEdge;
    Halfedge * nextOutEdge;
    Vector3f avgNorm = Vector3f(0, 0, 0);
    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;
        int n = 0;
        firstOutEdge = currVert -> oneOutEdge;
        nextOutEdge = firstOutEdge;
        do {
            if(nextOutEdge -> sibling == NULL && nextOutEdge -> mobiusSibling == NULL) {
                avgNorm += getNormal(nextOutEdge -> previousBoundary);
                nextOutEdge = nextOutEdge -> previousBoundary -> next;   
            } else {
                avgNorm += getNormal(nextOutEdge -> sibling);
                nextOutEdge = nextOutEdge -> sibling -> next;
            }
            n += 1;
        } while ( nextOutEdge != firstOutEdge);
        avgNorm /= n;
        avgNorm.normalize();
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
/*
    computeNormals(glMesh.VertVect);
*/
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
//          Let's build some Shapes!!
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

    e2->previous = e1;
    e3->previous = e2;
    e1->previous = e3;
    
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

    e2->previous = e1;
    e3->previous = e2;
    e4->previous = e3;
    e1->previous = e4;

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
void makeBoundary(vector<Vertex*> &oneBoundary, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect, bool isLoop){
    vector<Vertex*>::iterator bondIt;
    vector<Halfedge*> boundaryEdges;
    vector<Halfedge*>::iterator eIt;
    for(bondIt = oneBoundary.begin(); bondIt < oneBoundary.end(); bondIt ++) {
        for(eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt++) {
            if(bondIt != oneBoundary.end() - 1 && (*eIt) -> start == (*bondIt) && (*eIt) -> end == *(bondIt + 1)){
                boundaryEdges.push_back(*eIt);
            } else if(bondIt == oneBoundary.end() - 1 && (*eIt) -> start == (*bondIt) && (*eIt) -> end == *(oneBoundary.begin())){
                boundaryEdges.push_back(*eIt);
            }
        }
    }
    for(eIt = boundaryEdges.begin(); eIt < boundaryEdges.end(); eIt ++) {
        (*eIt) -> isSharp = true;
        if(eIt == boundaryEdges.begin()){
            (*eIt) -> previousBoundary = *(boundaryEdges.end() - 1);
            (*eIt) -> nextBoundary = *(eIt + 1);
        } else if(eIt == boundaryEdges.end() - 1){
            (*eIt) -> previousBoundary = *(eIt - 1);
            (*eIt) -> nextBoundary = *(boundaryEdges.begin());
        } else {
            (*eIt) -> previousBoundary = *(eIt - 1);
            (*eIt) -> nextBoundary = *(eIt + 1);
        }
    }
    cout<<boundaryEdges.size()<<endl;
}
void makeWithSIF(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect, string inputSIF){
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

    ifstream file(inputSIF);
    if (!file.good()) {
        exit(1); // exit if file not found
    }
    string nextLine;
    regex anyR(".*(.*).*");
    regex vRegex(".*\(v .*$\).*");
    regex tRegex(".*\(t .*\).*");
    regex lRegex(".*\(loop .*\).*");
    int vCounter = 0;
    vector<vector<int> > boundaries;
    while(getline(file, nextLine)){
        nextLine.pop_back();
        if(regex_match(nextLine, vRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            float x = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float y = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float z = stof(temp);
            Vertex * newVert = new Vertex;
            newVert -> position = Vector3f(x, y, z)*5 - Vector3f(0, 0, 4); // Can be modifed here to zoom in.
            newVert -> ID = vCounter;
            //cout<<newVert -> ID<<"Vertex added"<<endl;
            vertVect.push_back(newVert);
            vCounter += 1;
        } else if(regex_match(nextLine, tRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            //cout<<temp<<endl;
            temp = temp.substr(temp.find(" ") + 1);
            int a = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int b = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int c = stoi(temp);
            Vertex * va = vertVect[a];
            Vertex * vb = vertVect[b];
            Vertex * vc = vertVect[c];
            //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            makeTriFace(va, vb, vc, faceVect, edgeVect);
        } else if(regex_match(nextLine, lRegex)){
            vector<int> oneBoundary;
            string temp;
            int nextVert;
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            //cout<<temp<<endl;
            temp = temp.substr(temp.find(" ") + 1);
            while(temp.find(" ") != string::npos){
                nextVert = stoi(temp.substr(0, temp.find(" ")));
                oneBoundary.push_back(nextVert);
                temp = temp.substr(temp.find(" ") + 1);
            }
            nextVert = stoi(temp);
            oneBoundary.push_back(nextVert);
            boundaries.push_back(oneBoundary);
            //cout<<oneBoundary.size()<<endl;          
        }
    }
    //Siblings
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
    //Boundaries
    vector<vector<int> >::iterator bondIt;
    for( bondIt = boundaries.begin(); bondIt < boundaries.end(); bondIt ++) {
        vector<Vertex*> oneBoundary;
        vector<int>::iterator oneBondIt;
        for( oneBondIt = (*bondIt).end() - 1; oneBondIt >= (*bondIt).begin(); oneBondIt--){
            oneBoundary.push_back(vertVect[*oneBondIt]);
        }
        makeBoundary(oneBoundary, edgeVect, vertVect, true);
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

void initRendering();

void init(int level, string inputSIF){
    makeWithSIF(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect, inputSIF);
    cout<<glMesh.FaceVect.size()<<" "<< glMesh.VertVect.size()<<endl;
    for(int i = 0; i < level; i++) {
        ccSubDivision();
    }

}

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
    gluLookAt(3, 3, 3, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction
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
    viewport.width = 1280;
    viewport.hight = 720;
    int level = stoi(argv[1]);
    string inputSIF = argv[2];
    init(level, inputSIF);

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

    glutMainLoop();

}


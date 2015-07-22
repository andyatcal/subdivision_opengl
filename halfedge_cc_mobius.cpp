// Catmull-Clark Subdivision Project
// With sharp edge feature.
// With Mobius edge feature.
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
        cout<<"FacePoint: "<<endl;
        cout<<newFacePointPosition;
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
                cout<<"sharp edge starting from vertex"<<(*it) -> start -> ID<<endl;
                cout<<(*newEdgePoint).position<<endl;
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
                cout<<"normal edge starting from vertex"<<(*it) -> start -> ID<<endl;
                cout<<(*newEdgePoint).position<<endl;
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
                cout<<"mobius edge starting from vertex"<<(*it) -> start -> ID<<endl;
                cout<<(*newEdgePoint).position<<endl;
                (*it) -> edgePoint = newEdgePoint;
                vertVect.push_back(newEdgePoint);
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
        cout<<"vertexID: "<<currVert -> ID<<endl;
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
            newEdgePointAvgPoistion += nextOutEdge -> edgePoint -> position;
            newFacePointAvgPosition += nextOutEdge -> heFace -> facePoint -> position;
            n += 1;
            if(nextOutEdge -> isSharp) {
                //cout<<"A"<<endl;
                sharpEdgeCounter += 1;
                if(sharpEdgeCounter == 1) {
                    sharpEdgeI = nextOutEdge;
                    cout<<"HereA"<<endl;
                    cout<<"I start from: "<<sharpEdgeI -> start -> ID<<endl;
                    cout<<"I end at: "<<sharpEdgeI -> end -> ID<<endl;
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
                            cout<<"HereB"<<endl;
                        } else if(sharpEdgeCounter == 2) {
                            sharpEdgeK = nextOutEdge -> previousBoundary;
                            cout<<"HereC"<<endl;
                                                cout<<"K start from: "<<sharpEdgeK -> start -> ID<<endl;
                    cout<<"K end at: "<<sharpEdgeK -> end -> ID<<endl;
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
                            cout<<"HereD"<<endl;
                        } else if(sharpEdgeCounter == 2) {
                            sharpEdgeK = nextOutEdge -> mobiusBoundary;
                            cout<<"HereE"<<endl;
                        cout<<"K start from: "<<sharpEdgeK -> start -> ID<<endl;
                    cout<<"K end at: "<<sharpEdgeK -> end -> ID<<endl;
                        }
                        if(mobiusCounter % 2 == 0) {
                            nextOutEdge = nextOutEdge -> mobiusBoundary -> next;
                        } else {
                            nextOutEdge = nextOutEdge -> mobiusBoundary -> previous;
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
            cout<<"this is a normal vertex!"<<currVert -> position<<endl;            
        } else if(sharpEdgeCounter == 2) {
                                cout<<"I start from: "<<sharpEdgeI -> start -> ID<<endl;
                    cout<<"I end at: "<<sharpEdgeI -> end -> ID<<endl;
                                        cout<<"K start from: "<<sharpEdgeK -> start -> ID<<endl;
                    cout<<"K end at: "<<sharpEdgeK -> end -> ID<<endl;
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
            cout<<"pointI: "<<pointI -> position<<endl;
            cout<<"pointK: "<<pointK -> position<<endl;
            currVert -> position = (pointI -> copy + pointK -> copy + 6 * currVert -> copy) / 8;
            cout<<"this is a crease vertex!"<<currVert -> position<<endl;
        } else {
            currVert -> position = currVert -> position; // Nothing happens when sharp edges is more than 3.
            cout<<"this is a conner vertex!"<<currVert -> position<<endl;
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
                if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
                    edgeB -> previousBoundary = edgeA;
                    edgeA -> nextBoundary = edgeB;
                    if(currEdge -> previousBoundary -> secondHalf != NULL) {
                        currEdge -> previousBoundary -> secondHalf -> nextBoundary = edgeA;
                        edgeA -> previousBoundary = currEdge -> previousBoundary -> secondHalf;
                    }
                    if(currEdge -> nextBoundary -> firstHalf != NULL) {
                        currEdge -> nextBoundary -> firstHalf -> previousBoundary = edgeB;
                        edgeB -> nextBoundary = currEdge -> nextBoundary -> firstHalf;
                    }
                }
            }
            if(currEdge -> sibling != NULL && currEdge -> sibling -> firstHalf != NULL) {
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

    return oneEdge.cross(secondEdge);
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
            if(nextOutEdge -> sibling == NULL) {
                //cout<<"It has no sibling!"<<endl;
                avgNorm += getNormal(nextOutEdge -> previousBoundary);
                nextOutEdge = nextOutEdge -> previousBoundary -> next;
            } else {
                //cout<<"It has sibling!"<<endl;
                avgNorm += getNormal(nextOutEdge -> sibling);
                nextOutEdge = nextOutEdge -> sibling -> next;
            }
            n += 1;
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
    /*
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
    */
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
            Halfedge * temp = *(edges.begin());
            temp -> previous = currEdge;
        } else {
            currEdge -> next = *(eIt + 1);
            Halfedge * temp = *(eIt + 1);
            temp -> previous = currEdge;
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

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeOpenCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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

    //without the topFace
    //makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
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

    //Boundaries

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;
    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

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

void makeRing(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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

    //without the topFace
    //makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    //makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    //Boundaries

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

    e56 -> previousBoundary = e85;
    e56 -> nextBoundary = e67;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e56;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e56;
    e67 -> nextBoundary = e78;

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

void makeSharpCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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

    //without the topFace
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

    //Boundaries

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    Halfedge * e12;
    Halfedge * e41;
    Halfedge * e34;
    Halfedge * e23;

    Halfedge * e65;
    Halfedge * e76;
    Halfedge * e87;
    Halfedge * e58;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v1) {
            e41 = *eIt;
        }
        if((*eIt) -> start == v1 && (*eIt) -> end == v2) {
            e12 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v3) {
            e23 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v4) {
            e34 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v5) {
            e65 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v6) {
            e76 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v7) {
            e87 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v8) {
            e58 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e12 -> isSharp = true;
    e41 -> isSharp = true;
    e34 -> isSharp = true;
    e23 -> isSharp = true;

    e65 -> isSharp = true;
    e76 -> isSharp = true;
    e87 -> isSharp = true;
    e58 -> isSharp = true;

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

void makeOctahedron(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
 
    v1 -> position = Vector3f(0, 0, 1.414);
    v2 -> position = Vector3f(1, 1, 0);
    v3 -> position = Vector3f(1, -1, 0);
    v4 -> position = Vector3f(-1, -1, 0);
    v5 -> position = Vector3f(-1, 1, 0);
    v6 -> position = Vector3f(0, 0, -1.414);


    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    makeTriFace(v1, v5, v2, faceVect, edgeVect);
    makeTriFace(v6, v3, v2, faceVect, edgeVect);
    makeTriFace(v6, v4, v3, faceVect, edgeVect);
    makeTriFace(v6, v5, v4, faceVect, edgeVect);
    makeTriFace(v6, v2, v5, faceVect, edgeVect);

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

void makeSharpOctahedron(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
 
    v1 -> position = Vector3f(0, 0, 1.4);
    v2 -> position = Vector3f(1, 1, 0);
    v3 -> position = Vector3f(1, -1, 0);
    v4 -> position = Vector3f(-1, -1, 0);
    v5 -> position = Vector3f(-1, 1, 0);
    v6 -> position = Vector3f(0, 0, -1.4);


    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    makeTriFace(v1, v5, v2, faceVect, edgeVect);
    makeTriFace(v6, v3, v2, faceVect, edgeVect);
    makeTriFace(v6, v4, v3, faceVect, edgeVect);
    makeTriFace(v6, v5, v4, faceVect, edgeVect);
    makeTriFace(v6, v2, v5, faceVect, edgeVect);

    vector<Halfedge*>::iterator eIt;
    Halfedge * e23;
    Halfedge * e34;
    Halfedge * e45;
    Halfedge * e52;

    Halfedge * e32;
    Halfedge * e43;
    Halfedge * e54;
    Halfedge * e25;

    Halfedge * e13;
    Halfedge * e36;
    Halfedge * e65;
    Halfedge * e51;

    Halfedge * e31;
    Halfedge * e63;
    Halfedge * e56;
    Halfedge * e15;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v2 && (*eIt) -> end == v3) {
            e23 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v4) {
            e34 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v5) {
            e45 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v2) {
            e52 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v4) {
            e54 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v5) {
            e25 = *eIt;
        }

        if((*eIt) -> start == v1 && (*eIt) -> end == v3) {
            e13 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v6) {
            e36 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v5) {
            e65 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v1) {
            e51 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v1) {
            e31 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v3) {
            e63 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v1 && (*eIt) -> end == v5) {
            e15 = *eIt;
        }
    }

    e23 -> isSharp = true;
    e34 -> isSharp = true;
    e45 -> isSharp = true;
    e52 -> isSharp = true;

    e32 -> isSharp = true;
    e43 -> isSharp = true;
    e54 -> isSharp = true;
    e25 -> isSharp = true;

    e13 -> isSharp = true;
    e36 -> isSharp = true;
    e65 -> isSharp = true;
    e51 -> isSharp = true;

    e31 -> isSharp = true;
    e63 -> isSharp = true;
    e56 -> isSharp = true;
    e15 -> isSharp = true;

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

void makeNormalStrip(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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
 
    v1 -> position = Vector3f(1.5, 1.5, 0);
    v2 -> position = Vector3f(1.5, -1.5, 0);
    v3 -> position = Vector3f(-1.5, -1.5, 0);
    v4 -> position = Vector3f(-1.5, 1.5, 0);
    v5 -> position = Vector3f(0.5, 0.5, 0);
    v6 -> position = Vector3f(0.5, -0.5, 0);
    v7 -> position = Vector3f(-0.5, -0.5, 0);
    v8 -> position = Vector3f(-0.5, 0.5, 0);

    makeRectFace(v1, v5, v6, v2, faceVect, edgeVect);
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);

    //Boundaries
    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

    e56 -> previousBoundary = e85;
    e56 -> nextBoundary = e67;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e56;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e56;
    e67 -> nextBoundary = e78;

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

void makeMobius(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
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
    Vertex * v9 = new Vertex;
    Vertex * vX = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
    vertVect.push_back(v9);
    vertVect.push_back(vX);

    v1 -> position = Vector3f(1.5, 1.5, 0);
    v2 -> position = Vector3f(1.5, -1.5, 0);
    v3 -> position = Vector3f(-1.5, -1.5, 0);
    v4 -> position = Vector3f(-1.5, 1.5, 0);
    v5 -> position = Vector3f(0.5, 0.5, 0);
    v6 -> position = Vector3f(0.5, -0.5, 0);
    v7 -> position = Vector3f(-0.5, -0.5, 0);
    v8 -> position = Vector3f(-0.5, 0.5, 0);
    v9 -> position = Vector3f(1, 0, 0.5);
    vX -> position = Vector3f(1, 0, -0.5);

    v1 -> ID = 1;
    v2 -> ID = 2;
    v3 -> ID = 3;
    v4 -> ID = 4;
    v5 -> ID = 5;
    v6 -> ID = 6;
    v7 -> ID = 7;
    v8 -> ID = 8;
    v9 -> ID = 9;
    vX -> ID = 10;

    v9 -> onMobiusSibling = true;
    vX -> onMobiusSibling = true;

    makeRectFace(v1, v5, vX, v9, faceVect, edgeVect);
    makeRectFace(vX, v9, v6, v2, faceVect, edgeVect);
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);

    //Boundaries
    vector<Halfedge*>::iterator eIt;

    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    //Special case at Mobius edge
    Halfedge * e2X;
    Halfedge * e5X;

    Halfedge * e85;
    Halfedge * e78;
    Halfedge * e67;

    //Special case at Mobius edge
    Halfedge * e96;
    Halfedge * e91;

    //Mobius edge
    Halfedge * eX9_1;
    Halfedge * eX9_2;

    bool oneMobiusAdded = false;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == vX) {
            e2X = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == vX) {
            e5X = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
        if((*eIt) -> start == v9 && (*eIt) -> end == v1) {
            e91 = *eIt;
        }
        if((*eIt) -> start == v9 && (*eIt) -> end == v6) {
            e96 = *eIt;
        }
        if((*eIt) -> start == vX && (*eIt) -> end == v9 && !oneMobiusAdded) {
            eX9_1 = *eIt;
            oneMobiusAdded = true;
        } else if((*eIt) -> start == vX && (*eIt) -> end == v9) {
            eX9_2 = *eIt;
        }
    }

    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;
    e2X -> isSharp = true;
    e5X -> isSharp = true;

    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;
    e96 -> isSharp = true;
    e91 -> isSharp = true;


    e2X -> previousBoundary = e32;
    e14 -> previousBoundary = e91;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e2X;
    e5X -> previousBoundary = e85;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e5X;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e96;
    e67 -> nextBoundary = e78;
    e96 -> nextBoundary = e67;

    e2X -> mobiusBoundary = e5X;
    e5X -> mobiusBoundary = e2X;
    e96 -> mobiusBoundary = e91;
    e91 -> mobiusBoundary = e96;

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

    //Mobius sibling
    eX9_1 -> mobiusSibling = eX9_2;
    eX9_2 -> mobiusSibling = eX9_1;

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
    //makePyramid(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeSharpOctahedron(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeOpenCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeRing(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeSharpCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    makeMobius(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //cout<< glMesh.FaceVect.size()<<" "<<glMesh.EdgeVect.size()<<" "<<glMesh.VertVect.size();
    //computeNormals(glMesh.VertVect);
    //ccSubDivision();
    //for(int i = 0; i < level; i++) {
    //    ccSubDivision();
    //}
    ccSubDivision();
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5, 5, 5, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction
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
    int level = stoi(argv[1]);
    init(level);
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

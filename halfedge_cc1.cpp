// Catmull-Clark Subdivision Project
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
};

Halfedge::Halfedge(){
    start = end = NULL;
    sibling = NULL;
    next = previous = NULL;
    heFace = NULL;
    edgePoint = NULL;
    firstHalf = secondHalf = NULL;
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
        Vector3f newFacePointPosition;
        vector<Vertex*>::iterator vIt;
        if(!vertices.empty()) {
            for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
                newFacePointPosition += (*vIt) -> position;
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
        
        edge1 = currVert -> oneOutEdge;
        edge2 = edge1->next->next->next->sibling;
        edge3 = edge2->next->next->next->sibling;
        edge4 = edge3->next->next->next->sibling;
        edge5 = edge4->next->next->next->sibling;

        edgePoint1 = *(edge1->edgePoint); 
        edgePoint2 = *(edge2->edgePoint); 
        edgePoint3 = *(edge3->edgePoint); 
        edgePoint4 = *(edge4->edgePoint); 
        edgePoint5 = *(edge5->edgePoint); 

        facePoint1 = *(edge1->heFace->facePoint);
        facePoint2 = *(edge2->heFace->facePoint);
        facePoint3 = *(edge3->heFace->facePoint);
        facePoint4 = *(edge4->heFace->facePoint);
        facePoint5 = *(edge5->heFace->facePoint);
    
        int n;

        if (edge1 == edge4){
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x)/3;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y)/3;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z)/3;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x)/3;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y)/3;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z)/3;

            n = 4;
        } else if(edge1 == edge5){
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x)/4;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y)/4;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z)/4;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x)/4;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y)/4;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z)/4;

            n = 4;
        } else { // The polygons have less than or equal to 5 edges
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x + edgePoint5.x)/5;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y + edgePoint5.y)/5;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z + edgePoint5.z)/5;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x + facePoint5.x)/5;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y + facePoint5.y)/5;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z + facePoint5.z)/5;

            n = 5;
        }
        // Update the vertex point
        currVert->x = ((n - 3)*currVert->x +  2*edgePointAvg.x + facePointAvg.x)/n; 
        currVert->y = ((n - 3)*currVert->y +  2*edgePointAvg.y + facePointAvg.y)/n; 
        currVert->z = ((n - 3)*currVert->z +  2*edgePointAvg.z + facePointAvg.z)/n; 
    }
}
/*
void makeVertexPoints1(vector<Vertex*> &vertVect){
    vector<Vertex*>::iterator it;

    Halfedge * edge1;
    Halfedge * edge2;
    Halfedge * edge3;
    Halfedge * edge4;
    Halfedge * edge5;

    Vertex facePoint1;
    Vertex facePoint2;
    Vertex facePoint3;
    Vertex facePoint4;
    Vertex facePoint5;
    Vertex facePointAvg;

    Vertex edgePoint1;
    Vertex edgePoint2;
    Vertex edgePoint3;
    Vertex edgePoint4;
    Vertex edgePoint5;
    Vertex edgePointAvg;

    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;

        edge1 = currVert -> oneOutEdge;
        edge2 = edge1->next->next->next->sibling;
        edge3 = edge2->next->next->next->sibling;
        edge4 = edge3->next->next->next->sibling;
        edge5 = edge4->next->next->next->sibling;

        edgePoint1 = *(edge1->edgePoint); 
        edgePoint2 = *(edge2->edgePoint); 
        edgePoint3 = *(edge3->edgePoint); 
        edgePoint4 = *(edge4->edgePoint); 
        edgePoint5 = *(edge5->edgePoint); 

        facePoint1 = *(edge1->heFace->facePoint);
        facePoint2 = *(edge2->heFace->facePoint);
        facePoint3 = *(edge3->heFace->facePoint);
        facePoint4 = *(edge4->heFace->facePoint);
        facePoint5 = *(edge5->heFace->facePoint);
    
        int n;

        if (edge1 == edge4){
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x)/3;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y)/3;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z)/3;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x)/3;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y)/3;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z)/3;

            n = 4;
        } else if(edge1 == edge5){
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x)/4;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y)/4;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z)/4;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x)/4;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y)/4;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z)/4;

            n = 4;
        } else { // The polygons have less than or equal to 5 edges
            edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x + edgePoint5.x)/5;
            edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y + edgePoint5.y)/5;
            edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z + edgePoint5.z)/5;

            facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x + facePoint5.x)/5;
            facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y + facePoint5.y)/5;
            facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z + facePoint5.z)/5;

            n = 5;
        }
        // Update the vertex point
        currVert->x = ((n - 3)*currVert->x +  2*edgePointAvg.x + facePointAvg.x)/n; 
        currVert->y = ((n - 3)*currVert->y +  2*edgePointAvg.y + facePointAvg.y)/n; 
        currVert->z = ((n - 3)*currVert->z +  2*edgePointAvg.z + facePointAvg.z)/n; 
    }
}

// Create conections between the new points
void compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect, vector<Halfedge*> &newEdgeVect){
    vector<Face*>::iterator it;

    for(it = faceVect.begin(); it < faceVect.end(); it++){
        Face currFace = **it;

        Halfedge * edge1 = currFace.oneSideEdge;
        Halfedge * edge2 = edge1->next;
        Halfedge * edge3 = edge2->next;
        Halfedge * edge4 = edge3->next;

        Halfedge * edge1a = new Halfedge;
        Halfedge * edge1b = new Halfedge;
        Halfedge * edge2a = new Halfedge;
        Halfedge * edge2b = new Halfedge;
        Halfedge * edge3a = new Halfedge;
        Halfedge * edge3b = new Halfedge;
        Halfedge * edge4a = new Halfedge;
        Halfedge * edge4b = new Halfedge;

        Halfedge * edge1in = new Halfedge;
        Halfedge * edge1out = new Halfedge;
        Halfedge * edge2in = new Halfedge;
        Halfedge * edge2out = new Halfedge;
        Halfedge * edge3in = new Halfedge;
        Halfedge * edge3out = new Halfedge;
        Halfedge * edge4in = new Halfedge;
        Halfedge * edge4out = new Halfedge;

        //border edges 
        edge1->firstHalf = edge1a; 
        edge1->secondHalf = edge1b;
        edge2->firstHalf = edge2a; 
        edge2->secondHalf = edge2b;
        edge3->firstHalf = edge3a; 
        edge3->secondHalf = edge3b;
        edge4->firstHalf = edge4a; 
        edge4->secondHalf = edge4b;

        //replace edge1
        edge1a->start = edge1->start;
        edge1a->end = edge1->edgePoint;
        edge1b->start = edge1->edgePoint;
        edge1b->end = edge1->end; 
        if(edge1->sibling->firstHalf != NULL){
            edge1b->sibling = edge1->sibling->firstHalf;
            edge1a->sibling = edge1->sibling->secondHalf;
            
            edge1->sibling->firstHalf->sibling = edge1b;
            edge1->sibling->secondHalf->sibling = edge1a;
        }
        //adjust edge pointer from start vertex
        edge1->start->oneOutEdge = edge1a;
        //inner edge from edge 1
        edge1in->start = edge1->edgePoint;
        edge1in->end = currFace.facePoint;
        edge1out->end = edge1->edgePoint;
        edge1out->start = currFace.facePoint;
        edge1in->sibling = edge1out;
        edge1out->sibling = edge1in;
        //set edge pointer from edge1 vertex
        edge1->edgePoint->oneOutEdge = edge1in;

        //replace edge2
        edge2a->start = edge2->start;
        edge2a->end = edge2->edgePoint;
        edge2b->start = edge2->edgePoint;
        edge2b->end = edge2->end; 
        if(edge2->sibling->firstHalf != NULL){
            edge2b->sibling = edge2->sibling->firstHalf;
            edge2a->sibling = edge2->sibling->secondHalf;
            edge2->sibling->firstHalf->sibling = edge2b;
            edge2->sibling->secondHalf->sibling = edge2a;
        }
        //adjust edge pointer from start vertex
        edge2->start->oneOutEdge = edge2a;
        //inner edge from edge 2
        edge2in->start = edge2->edgePoint;
        edge2in->end = currFace.facePoint;
        edge2out->end = edge2->edgePoint;
        edge2out->start = currFace.facePoint;
        edge2in->sibling = edge2out;
        edge2out->sibling = edge2in;
        //set edge pointer from edge2 vertex
        edge2->edgePoint->oneOutEdge = edge2in;


        //replace edge3
        edge3a->start = edge3->start;
        edge3a->end = edge3->edgePoint;
        edge3b->start = edge3->edgePoint;
        edge3b->end = edge3->end; 
        if(edge3->sibling->firstHalf != NULL){
            edge3b->sibling = edge3->sibling->firstHalf;
            edge3a->sibling = edge3->sibling->secondHalf;
            edge3->sibling->firstHalf->sibling = edge3b;
            edge3->sibling->secondHalf->sibling = edge3a;
        }
        //adjust edge pointer from start vertex
        edge2->start->oneOutEdge = edge2a;
        //inner edge from edge 3
        edge3in->start = edge3->edgePoint;
        edge3in->end = currFace.facePoint;
        edge3out->end = edge3->edgePoint;
        edge3out->start = currFace.facePoint;
        edge3in->sibling = edge3out;
        edge3out->sibling = edge3in;
        //set edge pointer from edge3 vertex
        edge3->edgePoint->oneOutEdge = edge3in;


        //replace edge4
        edge4a->start = edge4->start;
        edge4a->end = edge4->edgePoint;
        edge4b->start = edge4->edgePoint;
        edge4b->end = edge4->end; 
        if(edge4->sibling->firstHalf != NULL){
            edge4b->sibling = edge4->sibling->firstHalf;
            edge4a->sibling = edge4->sibling->secondHalf;
            //modify so that BOTH sets of half edges have
            edge4->sibling->firstHalf->sibling = edge4b;
            edge4->sibling->secondHalf->sibling = edge4a;
        }
        //adjust edge pointer from start vertex
        edge4->start->oneOutEdge = edge4a;
        //inner edge from edge 4
        edge4in->start = edge4->edgePoint;
        edge4in->end = currFace.facePoint;
        edge4out->end = edge4->edgePoint;
        edge4out->start = currFace.facePoint;
        edge4in->sibling = edge4out;
        edge4out->sibling = edge4in;
        //set edge pointer from edge4 vertex
        edge4->edgePoint->oneOutEdge = edge4in;

        //set face vertex edge pointer
        currFace.facePoint->oneOutEdge = edge4out;

        //set all next pointers
        edge1a->next = edge1in;
        edge1in->next = edge4out;
        edge4out->next = edge4b; 
        edge4b->next = edge1a;

        edge2a->next = edge2in;
        edge2in->next = edge1out;
        edge1out->next = edge1b; 
        edge1b->next = edge2a;

        edge3a->next = edge3in;
        edge3in->next = edge2out;
        edge2out->next = edge2b; 
        edge2b->next = edge3a;

        edge4a->next = edge4in;
        edge4in->next = edge3out;
        edge3out->next = edge3b; 
        edge3b->next = edge4a;
            
        //create new faces/connect all face pointers
        Face *face1 = new Face;
        face1->v1 = edge1a->start;
        edge1a->heFace = face1;
        face1->v2 = edge1in->start;
        edge1in->heFace = face1;
        face1->v3 = edge4out->start;
        edge4out->heFace = face1;
        face1->v4 = edge4b->start;
        edge4b->heFace = face1;
        face1->oneSideEdge = edge4b;

        Face *face2 = new Face;
        face2->v1 = edge2a->start;
        edge2a->heFace = face2;
        face2->v2 = edge2in->start;
        edge2in->heFace = face2;
        face2->v3 = edge1out->start;
        edge1out->heFace = face2;
        face2->v4 = edge1b->start;
        edge1b->heFace = face2;
        face2->oneSideEdge = edge1b;

        Face *face3 = new Face;
        face3->v1 = edge3a->start;
        edge3a->heFace = face3;
        face3->v2 = edge3in->start;
        edge3in->heFace = face3;
        face3->v3 = edge2out->start;
        edge2out->heFace = face3;
        face3->v4 = edge2b->start;
        edge2b->heFace = face3;
        face3->oneSideEdge = edge2b;

        Face *face4 = new Face;
        face4->v1 = edge4a->start;
        edge4a->heFace = face4;
        face4->v2 = edge4in->start;
        edge4in->heFace = face4;
        face4->v3 = edge3out->start;
        edge3out->heFace = face4;
        face4->v4 = edge3b->start;
        edge3b->heFace = face4;
        face4->oneSideEdge = edge3b;

        //load each face into the new face vector
        newFaceVect.push_back(face1); 
        newFaceVect.push_back(face2); 
        newFaceVect.push_back(face3); 
        newFaceVect.push_back(face4); 

        //push all new edges onto the new edge vector
        newEdgeVect.push_back(edge1a);
        newEdgeVect.push_back(edge1b);
        newEdgeVect.push_back(edge2a);
        newEdgeVect.push_back(edge2b);
        newEdgeVect.push_back(edge3a);
        newEdgeVect.push_back(edge3b);
        newEdgeVect.push_back(edge4a);
        newEdgeVect.push_back(edge4b);

        newEdgeVect.push_back(edge1in);
        newEdgeVect.push_back(edge1out);
        newEdgeVect.push_back(edge2in);
        newEdgeVect.push_back(edge2out);
        newEdgeVect.push_back(edge3in);
        newEdgeVect.push_back(edge3out);
        newEdgeVect.push_back(edge4in);
        newEdgeVect.push_back(edge4out);

    }
}

//compute the normal vector of the vertex at the end of the input half edge
Vector3f getNormal(Halfedge currEdge){
    Vertex v1 = *(currEdge.start);
    Vertex v2 = *(currEdge.end);
    Vertex v3 = *((currEdge.next)->end);

    Vector3f outNorm;

    Vector3f oneEdge = Vector3f((v2.x - v1.x),(v2.y - v1.y),(v2.z - v1.z));
    Vector3f secondEdge = Vector3f((v3.x - v2.x), (v3.y - v2.y),(v3.z - v2.z));

    return oneEdge.cross(secondEdge);
}

//iterate over every vertex in the mesh and compute its normal
void computeNormals(vector<Vertex*> &vertVect){

    vector<Vertex*>::iterator it;

    Halfedge* edge1;
    Halfedge* edge2;
    Halfedge* edge3;
    Halfedge* edge4;
    Halfedge* edge5;

    Vector3f norm1;
    Vector3f norm2;
    Vector3f norm3;
    Vector3f norm4;
    Vector3f norm5;
    Vector3f avgNorm;

    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){

        currVert = *it;

        edge1 = currVert->oneOutEdge;

        edge2 = edge1->next->next->next->sibling;

        edge3 = edge2->next->next->next->sibling;

        edge4 = edge3->next->next->next->sibling;
        edge5 = edge4->next->next->next->sibling;

        norm1 = getNormal(*edge1);
        norm2 = getNormal(*edge2);
        norm3 = getNormal(*edge3);
        norm4 = getNormal(*edge4);
        norm5 = getNormal(*edge5);
       
        if(edge1 == edge4){
            avgNorm[0] =  (norm1[0] + norm2[0] + norm3[0])/3;
            avgNorm[1] =  (norm1[1] + norm2[1] + norm3[1])/3;
            avgNorm[2] =  (norm1[2] + norm2[2] + norm3[2])/3;
        } else if(edge1 == edge5){
            //average the face points and the edge points
            avgNorm[0] =  (norm1[0] + norm2[0] + norm3[0] + norm4[0])/4;
            avgNorm[1] =  (norm1[1] + norm2[1] + norm3[1] + norm4[1])/4;
            avgNorm[2] =  (norm1[2] + norm2[2] + norm3[2] + norm4[2])/4;
        } else{
            avgNorm[0] =  (norm1[0] + norm2[0] + norm3[0] + norm4[0] + norm5[0])/5;
            avgNorm[1] =  (norm1[1] + norm2[1] + norm3[1] + norm4[1] + norm5[1])/5;
            avgNorm[2] =  (norm1[2] + norm2[2] + norm3[2] + norm4[2] + norm5[2])/5;    
        }
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

void makeOneFace(Vertex * va, Vertex* vb, Vertex * vc, Vertex * vd, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3, *e4;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;
    e4 = new Halfedge;

    nextFace->v1 = va;
    nextFace->v2 = vb;
    nextFace->v3 = vc;
    nextFace->v4 = vd;

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
 
    v1->x = v2->x = v5->x = v6->x = 1;
    v4->x = v3->x = v8->x = v7->x = -1;

    v1->y = v4->y = v8->y = v5->y = 1;
    v2->y = v3->y = v7->y = v6->y = -1;

    v1->z = v2->z = v3->z = v4->z = 1;
    v5->z = v6->z = v7->z = v8->z = -1;

    //topFace
    makeOneFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    makeOneFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeOneFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeOneFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeOneFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeOneFace(v4, v3, v7, v8, faceVect, edgeVect);

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

//************************************************************
//          OpenGL Display Functions
//************************************************************
void init(int level);

void render(void);

void reshape(int w, int h);

void keyboard(unsigned char c, int x, int y);

void mouse(int button, int state, int x, int y);

void init(int level){
    makeCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //cout<< glMesh.FaceVect.size()<<" "<<glMesh.EdgeVect.size()<<" "<<glMesh.VertVect.size();
    //computeNormals(glMesh.VertVect);
    for(int i = 0; i < level; i++) {
        ccSubDivision();
    }
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5, 0, 0, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction

    vector<Face*>::iterator dispFaceIt;
    Face * tempFace;
    angle += 0.5;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 0, 0, 1);

    for(dispFaceIt = glMesh.FaceVect.begin(); dispFaceIt < glMesh.FaceVect.end(); dispFaceIt++){
        tempFace = *dispFaceIt;
        Vertex *tempv1 = tempFace->v1;
        Vertex *tempv2 = tempFace->v2;
        Vertex *tempv3 = tempFace->v3;
        Vertex *tempv4 = tempFace->v4;
        glBegin(GL_QUADS);

        glNormal3f(tempv1->normal[0],tempv1->normal[1],tempv1->normal[2]);
        glVertex3f(tempv1->x, tempv1->y, tempv1->z); 
        glNormal3f(tempv2->normal[0],tempv2->normal[1],tempv2->normal[2]);
        glVertex3f(tempv2->x, tempv2->y, tempv2->z); 
        glNormal3f(tempv3->normal[0],tempv3->normal[1],tempv3->normal[2]); 
        glVertex3f(tempv3->x,tempv3->y, tempv3->z); 
        glNormal3f(tempv4->normal[0],tempv4->normal[1],tempv4->normal[2]); 
        glVertex3f(tempv4->x, tempv4->y, tempv4->z); 
        
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
*/
//************************************************************
//          Main Function
//************************************************************

int main(int argc, char** argv) {
    /*
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    viewport.width = 640;
    viewport.hight = 480;
    init(2);
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
    */
}


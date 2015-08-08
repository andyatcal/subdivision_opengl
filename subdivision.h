// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __SUBDIVISION_H__
#define __SUBDIVISION_H__

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;
class Halfedge;
class Mesh;

//////////////////////////////////////////////////////////////////////
// Subdivision Class -- Functions to perform the subdivision for a mesh
class Subdivision{
public:
    // The mesh to be subdivide;
    Mesh currMesh;
    // Constructor.
    Subdivision();
    // Constructor. 
    // @param mesh: the reference of given mesh.
    Subdivision(Mesh & mesh);
    // The integration of subdivision.
    // @param level: level of Catmull-Clark subdivision.
    Mesh ccSubdivision(int level);
private:
    // Construct face points in Catmull-Clark subdivision.
    // Computed values are stored in face.facepoint. Add new Vertices to vertVect.
    // @param faceVect: List of faces in the mesh.
    // @param vertVect: List of vertices in the mesh to store new vertices.
    void makeFacePoints(vector<Face*> &faceVect, vector<Vertex*> &vertVect);
    // Construct edge points in Catmull-Clark subdivision.
    // Computed values are stored in edge.edgepoint. Add new Vertices to vertVect.
    // @param faceVect: List of edges in the mesh.
    // @param vertVect: List of vertices in the mesh to store new vertices.
    void makeEdgePoints(vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect);
    // Construct vertex points in Catmull-Clark subdivision.
    // By modifying the original position and pointers of the vertex.
    // @param vertVect: List of vertices in the mesh.
    void makeVertexPoints(vector<Vertex*> &vertVect);
    // Construct a new mesh after given new facepoints, edgepoints, and vertexpoints.
    // @param faceVect: List of faces in the mesh.
    // @param newFaceVect: the new list of faces in the mesh.
    // @param newEdgeVect: the new list of edges in the mesh.
    void compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect,
     vector<Halfedge*> &newEdgeVect);
};

Subdivision::Subdivision(){}

Subdivision::Subdivision(Mesh & mesh){
    currMesh = mesh;
}
void Subdivision::makeFacePoints(vector<Face*> &faceVect,
 vector<Vertex*> &vertVect){
    vector<Face*>::iterator it;
    for(it = faceVect.begin(); it < faceVect.end(); it++){
        vector<Vertex*> vertices = (*it) -> vertices;
        // Facepoint is the average of vertices in this face
        Vertex * newFacePoint = new Vertex;
        vec3 newFacePointPosition = vec3(0, 0, 0);
        vector<Vertex*>::iterator vIt;
        if(!vertices.empty()) {
            for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
                newFacePointPosition += ((*vIt) -> position);
            }
            newFacePointPosition /= vertices.size();
            newFacePoint -> position = newFacePointPosition;
            //cout<<newFacePointPosition<<endl;
        } else {
            cout << "ERROR: CAN'T CALCULATE FACEPOINTS WITH LESS THAN 0 VERTICES."<<endl;
            exit(0);
        }
        (*it) -> facePoint = newFacePoint;
        vertVect.push_back(newFacePoint);
        //cout<<"FacePoint: "<<endl;
        //cout<<newFacePointPosition;
    }
}

void Subdivision::makeEdgePoints(vector<Halfedge*> &edgeVect,
 vector<Vertex*> &vertVect){
    vector<Halfedge*>::iterator it;
    for(it = edgeVect.begin(); it < edgeVect.end(); it++){
        if((*it) -> isSharp) {
            if((*it) -> sibling != NULL && (*it) -> sibling -> edgePoint != NULL){
                (*it) -> edgePoint = (*it) -> sibling -> edgePoint;
            } else if((*it) -> mobiusSibling != NULL && (*it) -> mobiusSibling -> edgePoint
             != NULL){
                (*it) -> edgePoint = (*it) -> mobiusSibling -> edgePoint;
            } else {
                Vertex * newEdgePoint = new Vertex;
                (*newEdgePoint).position = ((*it) -> end -> position + (*it) -> start
                 -> position) / (float)2;
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
                (*newEdgePoint).position = (faceVert1.position + faceVert2.position
                 + edgeVert1.position + edgeVert2.position) / (float)4;
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
                (*newEdgePoint).position = (faceVert1.position + faceVert2.position + edgeVert1.position
                 + edgeVert2.position) / (float)4;
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

void Subdivision::makeVertexPoints(vector<Vertex*> &vertVect){
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

        vec3 newFacePointAvgPosition = vec3(0, 0, 0);
        vec3 newEdgePointAvgPoistion = vec3(0, 0, 0);
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
            currVert->position = ((float) (n - 2) * currVert->position + edgePointAvg.position +
             facePointAvg.position) / (float) n;
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
            currVert -> position = (pointI -> copy + pointK -> copy + 6.0f * currVert -> copy) / 8.0f;
            //cout<<"this is a crease vertex!"<<currVert -> position<<endl;
        } else {
            currVert -> position = currVert -> position; // Nothing happens when sharp edges is more than 3.
            //cout<<"this is a conner vertex!"<<currVert -> position<<endl;
        }
    }
}
void Subdivision::compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect,
 vector<Halfedge*> &newEdgeVect){
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

Mesh Subdivision::ccSubdivision(int level){
    for(int i = 0; i < level; i++) {
        Mesh newMesh;
        Halfedge * tempEdge;
        Face * tempFace;
        newMesh.vertVect = currMesh.vertVect;
        makeFacePoints(currMesh.faceVect, currMesh.vertVect);
        makeEdgePoints(currMesh.edgeVect, currMesh.vertVect);
        makeVertexPoints(newMesh.vertVect);
        compileNewMesh(currMesh.faceVect, newMesh.faceVect, newMesh.edgeVect);
        while(!currMesh.faceVect.empty()){
            tempFace = currMesh.faceVect.back();
            currMesh.faceVect.pop_back();
            delete tempFace;
        }
        while(!currMesh.edgeVect.empty()){
            tempEdge = currMesh.edgeVect.back();
            currMesh.edgeVect.pop_back();
            delete tempEdge;
        }
        currMesh.faceVect = newMesh.faceVect;
        currMesh.edgeVect = newMesh.edgeVect;
    }
    return currMesh;
}

#endif // __SUBDIVISION_H__
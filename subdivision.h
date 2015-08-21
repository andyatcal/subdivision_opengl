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

//////////////////////////////////////////////////////////////////////
// Subdivision Class -- Functions to perform the subdivision for a mesh
class Subdivision{
public:
    // The mesh to be subdivide;
    Mesh currMesh;
    // Constructor.
    Subdivision(){};
    // Constructor. 
    // @param mesh: the reference of given mesh.
    Subdivision(Mesh & mesh);
    // The integration of subdivision.
    // @param level: level of Catmull-Clark subdivision.
    Mesh ccSubdivision(int level);
private:
    // Construct face points in Catmull-Clark subdivision.
    // Computed values are stored in face.facepoint. Add new Vertices to vertTable.
    // @param faceVect: a vector of faces in the mesh.
    // @param vertTable: HashTable of vertices in the mesh to store new vertices.
    void makeFacePoints(vector<Face*> &faceVect, unordered_map<unsigned long, Vertex*> &vertTable);
    // Construct edge points in Catmull-Clark subdivision.
    // Computed values are stored in edge.edgepoint. Add new Vertices to vertTable.
    // @param vertTable: HashTable of vertices in the mesh to store new vertices.
    void makeEdgePoints(vector<Face*> &faceVect, unordered_map<unsigned long, Vertex*> &vertTable);
    // Construct vertex points in Catmull-Clark subdivision with DeRose et al's Equation.
    // By modifying the original position and pointers of the vertex.
    // @param vertTable: List of vertices in the mesh.
    void makeVertexPointsD(unordered_map<unsigned long, Vertex*> &vertTable);
    // Similar to the above function, but with Catmull-Clark's equation.
    // @param vertTable: List of vertices in the mesh.
    void makeVertexPointsC(unordered_map<unsigned long, Vertex*> &vertTable);
    // Construct a new mesh after given new facepoints, edgepoints, and vertexpoints.
    // @param faceVect: List of faces in the mesh.
    // @param newFaceVect: the new list of faces in the mesh.
    // @param newEdgeTable: the new list of edges in the mesh.
    void compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect);
};

//Subdivision::Subdivision(){}

Subdivision::Subdivision(Mesh & mesh){
    currMesh = mesh;
}

void Subdivision::makeFacePoints(vector<Face*> &faceVect,
 unordered_map<unsigned long, Vertex*> &vertTable){
    unsigned long currID = vertTable.size();
    vector<Face*>::iterator fIt;
    for(fIt = faceVect.begin(); fIt < faceVect.end(); fIt++){
        // Facepoint is the average of vertices in this face
        Vertex * newFacePoint = new Vertex;
        vec3 newFacePointPosition = vec3(0, 0, 0);
        Face * currFace = (*fIt);
        Halfedge * firstSideEdge = currFace -> oneSideEdge;
        if(firstSideEdge == NULL) {
            cout<<"ERROR: This face (with ID) does not have a sideEdge."<<endl;
            exit(1);
        }
        Halfedge * nextSideEdge = firstSideEdge;
        uint counter = 0;
        do {
            newFacePointPosition += nextSideEdge -> end -> position;
            nextSideEdge = nextSideEdge -> next;
            counter += 1;
        } while(nextSideEdge != firstSideEdge);
        newFacePointPosition /= counter;
        newFacePoint -> position = newFacePointPosition;
        newFacePoint -> ID = currID;
        currFace -> facePoint = newFacePoint;
        addVertexToMesh(newFacePoint, currMesh);
        currID += 1;
    }
}

void Subdivision::makeEdgePoints(vector<Face*> &faceVect,
 unordered_map<unsigned long, Vertex*> &vertTable) {
    unsigned long currID = vertTable.size();
    vector<Face*>::iterator fIt;
    for(fIt = faceVect.begin(); fIt < faceVect.end(); fIt++){
        Face * currFace = (*fIt);
        Halfedge * firstEdge = currFace -> oneSideEdge;
        Halfedge * currEdge = currFace -> oneSideEdge;
        do {
            Vertex * newEdgePoint = new Vertex;
            if(currEdge -> edgePoint == NULL) {
                if(currEdge -> isSharp) {
                    newEdgePoint -> position = (currEdge -> end -> position + currEdge -> start
                     -> position) / (float) 2.0;
                    if(currEdge -> sibling != NULL) {
                        currEdge -> sibling -> edgePoint = newEdgePoint;
                    } else if(currEdge -> mobiusSibling != NULL) {
                        currEdge -> mobiusSibling -> edgePoint = newEdgePoint;
                    }
                } else {
                    Vertex * faceVert1 = currEdge -> heFace -> facePoint;
                    Vertex * edgeVert1 = currEdge -> end;
                    Vertex * edgeVert2 = currEdge -> start;
                    Vertex * faceVert2;
                    if(currEdge -> sibling != NULL) {
                        faceVert2 = currEdge -> sibling -> heFace -> facePoint;
                        currEdge -> sibling -> edgePoint = newEdgePoint;
                    } else if(currEdge -> mobiusSibling != NULL) {
                        faceVert2 = currEdge -> mobiusSibling -> heFace -> facePoint;
                        newEdgePoint -> onMobiusSibling = true;
                        currEdge -> mobiusSibling -> edgePoint = newEdgePoint;
                    }
                    newEdgePoint -> position = (faceVert1 -> position + faceVert2 -> position
                     + edgeVert1 -> position + edgeVert2 -> position) / (float) 4.0;
                }
                currEdge -> edgePoint = newEdgePoint;
                newEdgePoint -> ID = currID;
                addVertexToMesh(newEdgePoint, currMesh);
                currID += 1;
            }
            currEdge = currEdge -> next;
        } while (currEdge != firstEdge);
    }
}

void Subdivision::makeVertexPointsC(unordered_map<unsigned long, Vertex*> &vertTable){
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    Vertex facePointAvg;
    Vertex edgePointAvg;
    Vertex * currVert;
    //make a copy of the original mesh vertex points.
    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++){
        vIt -> second -> makeCopy();
    }

    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++){
        currVert = vIt -> second;
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
        int en = 0;
        int fn = 0;
        do {
            //cout<<"here"<<endl<<nextOutEdge -> end -> ID<<endl;
            newEdgePointAvgPoistion += (nextOutEdge -> start -> position + nextOutEdge -> end -> position) / 2.0f;
            newFacePointAvgPosition += nextOutEdge -> heFace -> facePoint -> position;
            en += 1;
            fn += 1;
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
                        newEdgePointAvgPoistion += (nextOutEdge -> previousBoundary -> start -> position + nextOutEdge -> previousBoundary -> end -> position)/2.0f;
                        en += 1;
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

                        newEdgePointAvgPoistion += (nextOutEdge -> mobiusBoundary -> start -> position + nextOutEdge -> mobiusBoundary -> end -> position)/2.0f;
                        en += 1;
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
            newFacePointAvgPosition /= fn;
            newEdgePointAvgPoistion /= en;
            edgePointAvg.position = newEdgePointAvgPoistion;
            facePointAvg.position = newFacePointAvgPosition;
            currVert->position = ((float) (fn - 3) * currVert->position + 2.0f * edgePointAvg.position +
             facePointAvg.position) / (float) en;
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

void Subdivision::makeVertexPointsD(unordered_map<unsigned long, Vertex*> &vertTable){
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    Vertex facePointAvg;
    Vertex edgePointAvg;
    Vertex * currVert;
    //make a copy of the original mesh vertex points.
    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++){
        vIt -> second -> makeCopy();
    }

    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++){
        currVert = vIt -> second;
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
        int en = 0;
        int fn = 0;
        do {
            //cout<<"here"<<endl<<nextOutEdge -> end -> ID<<endl;
            newEdgePointAvgPoistion += nextOutEdge -> edgePoint -> position;
            newFacePointAvgPosition += nextOutEdge -> heFace -> facePoint -> position;
            en += 1;
            fn += 1;
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
                        en += 1;
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
            newFacePointAvgPosition /= fn;
            newEdgePointAvgPoistion /= en;
            edgePointAvg.position = newEdgePointAvgPoistion;
            facePointAvg.position = newFacePointAvgPosition;
            currVert->position = ((float) (en - 2) * currVert->position + edgePointAvg.position +
             facePointAvg.position) / (float) en;
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

void Subdivision::compileNewMesh(vector<Face*> &faceVect, vector<Face*> &newFaceVect){
    vector<Face*>::iterator fIt;
    for(fIt = faceVect.begin(); fIt < faceVect.end(); fIt++){
        Face * currFace = (*fIt);
        Halfedge * firstEdge = currFace -> oneSideEdge;
        Halfedge * currEdge = currFace -> oneSideEdge;
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
            }  else if(currEdge -> mobiusSibling != NULL && currEdge -> mobiusSibling -> firstHalf != NULL) {
                edgeA -> mobiusSibling = currEdge -> mobiusSibling -> firstHalf;
                edgeB -> mobiusSibling = currEdge -> mobiusSibling -> secondHalf;

                currEdge -> mobiusSibling -> firstHalf -> mobiusSibling = edgeA;
                currEdge -> mobiusSibling -> secondHalf -> mobiusSibling = edgeB;
            }
            currEdge -> start -> oneOutEdge = edgeA;
            edgeIn -> start = currEdge -> edgePoint;
            edgeIn -> end = currFace -> facePoint;
            edgeOut -> end = currEdge -> edgePoint;
            edgeOut -> start = currFace -> facePoint;
            edgeIn->sibling = edgeOut;
            edgeOut->sibling = edgeIn;
            currEdge -> edgePoint -> oneOutEdge = edgeIn;
            currFace -> facePoint -> oneOutEdge = edgeOut;
            edgeA -> next = edgeIn;
            edgeIn -> previous = edgeA;
            edgeIn -> next = previousOut;
            previousOut -> previous = edgeIn;
            previousOut -> next = previousB;
            previousB -> previous = previousOut;
            previousB -> next = edgeA;
            edgeA -> previous = previousB;
            newFace = new Face;
            edgeA -> heFace = newFace;
            edgeIn -> heFace = newFace;
            previousOut -> heFace = newFace;
            previousB -> heFace = newFace;
            newFace -> oneSideEdge = previousB;
            if(currEdge!= firstEdge){
                newFaceVect.push_back(newFace);
            }
            currEdge = currEdge -> next;
            previousB = edgeB;
            previousOut = edgeOut;
        } while (currEdge != firstEdge);
        //Connect the first and last
        edgeA = currFace -> oneSideEdge -> firstHalf;
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
        newMesh.vertTable = currMesh.vertTable;
        makeFacePoints(currMesh.faceVect, currMesh.vertTable);
        makeEdgePoints(currMesh.faceVect, currMesh.vertTable);
        makeVertexPointsD(newMesh.vertTable);
        compileNewMesh(currMesh.faceVect, newMesh.faceVect);
        vector<Face*>::iterator fIt;
        unordered_map<unsigned long long, Halfedge*>::iterator eIt;
        for(fIt = currMesh.faceVect.begin(); fIt != currMesh.faceVect.end(); fIt++) {
            tempFace = (*fIt);
            delete tempFace;
        }
        currMesh.faceVect.clear();
        currMesh.faceVect = newMesh.faceVect;
    }
    return currMesh;
}

#endif // __SUBDIVISION_H__
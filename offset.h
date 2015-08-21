// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __OFFSET_H__
#define __OFFSET_H__

#include <glm/glm.hpp>
#include <vector>

using namespace glm;

// Forward declarations
class Halfedge;
class Vertex;
class Face;
class Mesh;


//////////////////////////////////////////////////////////////////////
// Offset Class -- Offset for a given mesh.
//
class Offset{
public:
    // The amount of offset for this mesh.
    float offsetVal;
    // The original mesh.
    Mesh startMesh;
    // The positive offset mesh.
    Mesh posOffsetMesh;
    // The negtive offset mesh.
    Mesh negOffsetMesh;
    // The side offset mesh.
    Mesh sideOffsetMesh;
    // The combination of posOffsetMesh, negOffsetMesh and sideOffsetMesh
    Mesh offsetMesh;
    // Constructor of offset.
    // @param mesh: the initial mesh to find offset
    // @param val: the offsetVal
    Offset(Mesh & mesh, float val);
    // Find the positive and negative offset and their side cover.
    // It runs fast and with sharp edge between the cover and offset surface.
    void makeSeperateOffset();
    // Find the positive, negative and side cover as a whole mesh.
    // Runs slow then the seperate one, but have smooth connection between cover and offset surface.
    void makeFullOffset();
    // Change the value of offset
    // @param val: the offset value, can be positive or negative.
    void changeOffsetValTo(float val);


private:
    // Calculate the offset for one vertex.
    // @param v: the vertex to calculate offset.
    // @param full: true if we are doing a full offset. false if we are doing seperate offset.
    void calcVertexOffset(Vertex * v, bool full);
};

Offset::Offset(Mesh & mesh, float val) {
    offsetVal = val;
    startMesh = mesh;
}

void Offset::makeSeperateOffset() {
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    vector<Face*>::iterator fIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(vIt = startMesh.vertTable.begin(); vIt != startMesh.vertTable.end(); vIt++) {
        calcVertexOffset(vIt -> second, false);
    }
    for(fIt = startMesh.faceVect.begin(); fIt < startMesh.faceVect.end(); fIt++) {
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        Face * currFace = (*fIt);
        Halfedge * firstSideEdge = currFace -> oneSideEdge;
        if(firstSideEdge == NULL) {
            cout<<"ERROR: This face (with ID) does not have a sideEdge."<<endl;
            exit(1);
        }
        Halfedge * nextSideEdge = firstSideEdge;
        do {
            Vertex * currVert = nextSideEdge -> end;
            if(!(currVert -> onMobiusSibling)) {
                posOffsetVertices.push_back(currVert -> posOffset);
                negOffsetVertices.push_back(currVert -> negOffset);
            } else {
                vec3 oneOption = currVert -> posOffset -> position - currVert -> position;
                if(dot(oneOption, currFace -> faceNormal) >= 0) {
                    currVert -> negOffset -> normal = - currVert -> normal;
                    currVert -> posOffset -> normal = currVert -> normal;
                    posOffsetVertices.push_back(currVert -> posOffset);
                    negOffsetVertices.push_back(currVert -> negOffset);
                } else {
                    currVert -> negOffset -> normal = currVert -> normal;
                    currVert -> posOffset -> normal = - currVert -> normal;
                    posOffsetVertices.push_back(currVert -> negOffset);
                    negOffsetVertices.push_back(currVert -> posOffset);
                }
            }
            nextSideEdge = nextSideEdge -> next;
        } while(nextSideEdge != firstSideEdge);
        addPolygonFaceToMesh(posOffsetVertices, posOffsetMesh);
        addPolygonFaceToMesh(negOffsetVertices, negOffsetMesh, true);

        firstSideEdge = (*fIt) -> oneSideEdge;
        Halfedge * currEdge = firstSideEdge; 
        do {
            if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
                vector<Vertex*> sideOffsetVertices1;
                vector<Vertex*> sideOffsetVertices2;
                if(currEdge -> start -> onMobiusSibling) {
                    vec3 oneOption = currEdge -> start -> posOffset -> position - currEdge -> start -> position;
                    if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                        sideOffsetVertices1.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices1.push_back(currEdge -> start);
                        sideOffsetVertices1.push_back(currEdge -> end);
                        sideOffsetVertices1.push_back(currEdge -> end -> posOffset);
                        sideOffsetVertices2.push_back(currEdge -> start);
                        sideOffsetVertices2.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end);
                    } else {
                        sideOffsetVertices1.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices1.push_back(currEdge -> start);
                        sideOffsetVertices1.push_back(currEdge -> end);
                        sideOffsetVertices1.push_back(currEdge -> end -> posOffset);
                        sideOffsetVertices2.push_back(currEdge -> start);
                        sideOffsetVertices2.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices2.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end);
                    }
                } else if(currEdge -> end -> onMobiusSibling) {
                    vec3 oneOption = currEdge -> end -> posOffset -> position - currEdge -> end -> position;
                    if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                        sideOffsetVertices1.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices1.push_back(currEdge -> start);
                        sideOffsetVertices1.push_back(currEdge -> end);
                        sideOffsetVertices1.push_back(currEdge -> end -> posOffset);
                        sideOffsetVertices2.push_back(currEdge -> start);
                        sideOffsetVertices2.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end);
                    } else {
                        sideOffsetVertices1.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices1.push_back(currEdge -> start);
                        sideOffsetVertices1.push_back(currEdge -> end);
                        sideOffsetVertices1.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices2.push_back(currEdge -> end -> posOffset);
                        sideOffsetVertices2.push_back(currEdge -> end);
                        sideOffsetVertices2.push_back(currEdge -> start);
                    }
                } else {
                    sideOffsetVertices1.push_back(currEdge -> start -> posOffset);
                    sideOffsetVertices1.push_back(currEdge -> start);
                    sideOffsetVertices1.push_back(currEdge -> end);
                    sideOffsetVertices1.push_back(currEdge -> end -> posOffset);
                    sideOffsetVertices2.push_back(currEdge -> start);
                    sideOffsetVertices2.push_back(currEdge -> start -> negOffset);
                    sideOffsetVertices2.push_back(currEdge -> end -> negOffset);
                    sideOffsetVertices2.push_back(currEdge -> end);
                }
                addPolygonFaceToMesh(sideOffsetVertices1, sideOffsetMesh);
                addPolygonFaceToMesh(sideOffsetVertices2, sideOffsetMesh);
            }
            currEdge = currEdge -> next;
        } while (currEdge != firstSideEdge);
    }
}

void Offset::makeFullOffset() {
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    vector<Face*>::iterator fIt;
    for(vIt = startMesh.vertTable.begin(); vIt != startMesh.vertTable.end(); vIt++) {
        calcVertexOffset(vIt -> second, true);
    }
    for(fIt = startMesh.faceVect.begin(); fIt < startMesh.faceVect.end(); fIt++) {
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        Face * currFace = (*fIt);
        Halfedge * firstSideEdge = currFace -> oneSideEdge;
        if(firstSideEdge == NULL) {
            cout<<"ERROR: This face (with ID) does not have a sideEdge."<<endl;
            exit(1);
        }
        Halfedge * nextSideEdge = firstSideEdge;
        do {
            Vertex * currVert = nextSideEdge -> end;
            if(!(currVert -> onMobiusSibling)) {
                posOffsetVertices.push_back(currVert -> posOffset);
                negOffsetVertices.push_back(currVert -> negOffset);
            } else {
                vec3 oneOption = currVert -> posOffset -> position - currVert -> position;
                if(dot(oneOption, currFace -> faceNormal) >= 0) {
                    currVert -> negOffset -> normal = - currVert -> normal;
                    currVert -> posOffset -> normal = currVert -> normal;
                    posOffsetVertices.push_back(currVert -> posOffset);
                    negOffsetVertices.push_back(currVert -> negOffset);
                } else {
                    currVert -> negOffset -> normal = currVert -> normal;
                    currVert -> posOffset -> normal = - currVert -> normal;
                    posOffsetVertices.push_back(currVert -> negOffset);
                    negOffsetVertices.push_back(currVert -> posOffset);
                }
            }
            nextSideEdge = nextSideEdge -> next;
        } while(nextSideEdge != firstSideEdge);
        addPolygonFaceToMesh(posOffsetVertices, offsetMesh);
        addPolygonFaceToMesh(negOffsetVertices, offsetMesh, true);

        firstSideEdge = currFace -> oneSideEdge;
        Halfedge * currEdge = firstSideEdge;
        do {
            if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
                vector<Vertex*> sideOffsetVertices;
                if(currEdge -> start -> onMobiusSibling) {
                    vec3 oneOption = currEdge -> start -> posOffset -> position - currEdge -> start -> position;
                    if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                        sideOffsetVertices.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> posOffset);
                    } else {
                        sideOffsetVertices.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> posOffset);
                    }
                } else if(currEdge -> end -> onMobiusSibling) {
                    vec3 oneOption = currEdge -> end -> posOffset -> position - currEdge -> end -> position;
                    if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                        sideOffsetVertices.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> posOffset);
                    } else {
                        sideOffsetVertices.push_back(currEdge -> start -> posOffset);
                        sideOffsetVertices.push_back(currEdge -> start -> negOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> posOffset);
                        sideOffsetVertices.push_back(currEdge -> end -> negOffset);
                    }
                } else {
                    sideOffsetVertices.push_back(currEdge -> start -> posOffset);
                    sideOffsetVertices.push_back(currEdge -> start -> negOffset);
                    sideOffsetVertices.push_back(currEdge -> end -> negOffset);
                    sideOffsetVertices.push_back(currEdge -> end -> posOffset);
                }
                addPolygonFaceToMesh(sideOffsetVertices, offsetMesh);
            }
            currEdge = currEdge -> next;
        } while (currEdge != firstSideEdge);
    }
    buildConnections(offsetMesh);
    computeNormals(offsetMesh);
}

void Offset::calcVertexOffset(Vertex * v, bool full) {
    Vertex * posOffset = new Vertex;
    Vertex * negOffset = new Vertex;
    posOffset -> position = v -> position + v -> normal * offsetVal;
    negOffset -> position = v -> position - v -> normal * offsetVal;
    posOffset -> ID = v -> ID + 2000000;
    negOffset -> ID = v -> ID + 4000000;
    posOffset -> normal = v -> normal;
    negOffset -> normal = - v -> normal;
    v -> posOffset = posOffset;
    v -> negOffset = negOffset;
    if(!full) {
        addVertexToMesh(posOffset, posOffsetMesh);
        addVertexToMesh(negOffset, negOffsetMesh);
        addVertexToMesh(posOffset, negOffsetMesh);
        addVertexToMesh(negOffset, posOffsetMesh);
        addVertexToMesh(posOffset, sideOffsetMesh);
        addVertexToMesh(negOffset, sideOffsetMesh);
        addVertexToMesh(v, sideOffsetMesh);
    } else {
        addVertexToMesh(posOffset, offsetMesh);
        addVertexToMesh(negOffset, offsetMesh);
    }
}

#endif // __OFFSET_H__
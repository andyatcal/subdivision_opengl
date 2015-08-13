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
    unordered_map<uint, Face*>::iterator fIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(vIt = startMesh.vertTable.begin(); vIt != startMesh.vertTable.end(); vIt++) {
        calcVertexOffset(vIt -> second, false);
    }
    for(fIt = startMesh.faceTable.begin(); fIt != startMesh.faceTable.end(); fIt++) {
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        Face * currFace = fIt -> second;
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
        makePolygonFace(posOffsetVertices, posOffsetMesh.faceTable, posOffsetMesh.edgeTable);
        makePolygonFace(negOffsetVertices, negOffsetMesh.faceTable, negOffsetMesh.edgeTable, true);
    }
    for(eIt = startMesh.edgeTable.begin(); eIt != startMesh.edgeTable.end(); eIt++) {
        Halfedge * currEdge = eIt -> second;
        if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
            vector<Vertex*> offsetSideVertices1;
            vector<Vertex*> offsetSideVertices2;
            if(currEdge -> start -> onMobiusSibling) {
                vec3 oneOption = currEdge -> start -> posOffset -> position - currEdge -> start -> position;
                if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices1.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices1.push_back(currEdge -> start);
                    offsetSideVertices1.push_back(currEdge -> end);
                    offsetSideVertices1.push_back(currEdge -> end -> posOffset);
                    offsetSideVertices2.push_back(currEdge -> start);
                    offsetSideVertices2.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end);
                } else {
                    offsetSideVertices1.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices1.push_back(currEdge -> start);
                    offsetSideVertices1.push_back(currEdge -> end);
                    offsetSideVertices1.push_back(currEdge -> end -> posOffset);
                    offsetSideVertices2.push_back(currEdge -> start);
                    offsetSideVertices2.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices2.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end);
                }
            } else if(currEdge -> end -> onMobiusSibling) {
                vec3 oneOption = currEdge -> end -> posOffset -> position - currEdge -> end -> position;
                if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices1.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices1.push_back(currEdge -> start);
                    offsetSideVertices1.push_back(currEdge -> end);
                    offsetSideVertices1.push_back(currEdge -> end -> posOffset);
                    offsetSideVertices2.push_back(currEdge -> start);
                    offsetSideVertices2.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end);
                } else {
                    offsetSideVertices1.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices1.push_back(currEdge -> start);
                    offsetSideVertices1.push_back(currEdge -> end);
                    offsetSideVertices1.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices2.push_back(currEdge -> end -> posOffset);
                    offsetSideVertices2.push_back(currEdge -> end);
                    offsetSideVertices2.push_back(currEdge -> start);
                }
            } else {
                offsetSideVertices1.push_back(currEdge -> start -> posOffset);
                offsetSideVertices1.push_back(currEdge -> start);
                offsetSideVertices1.push_back(currEdge -> end);
                offsetSideVertices1.push_back(currEdge -> end -> posOffset);
                offsetSideVertices2.push_back(currEdge -> start);
                offsetSideVertices2.push_back(currEdge -> start -> negOffset);
                offsetSideVertices2.push_back(currEdge -> end -> negOffset);
                offsetSideVertices2.push_back(currEdge -> end);
            }
            makePolygonFace(offsetSideVertices1, sideOffsetMesh.faceTable, sideOffsetMesh.edgeTable);
            makePolygonFace(offsetSideVertices2, sideOffsetMesh.faceTable, sideOffsetMesh.edgeTable);
        }
    }
    buildConnections(sideOffsetMesh);
    computeNormals(sideOffsetMesh);
}

void Offset::makeFullOffset() {
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    unordered_map<uint, Face*>::iterator fIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(vIt = startMesh.vertTable.begin(); vIt != startMesh.vertTable.end(); vIt++) {
        calcVertexOffset(vIt -> second, true);
    }
    for(fIt = startMesh.faceTable.begin(); fIt != startMesh.faceTable.end(); fIt++) {
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        Face * currFace = fIt -> second;
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
        makePolygonFace(posOffsetVertices, offsetMesh.faceTable, offsetMesh.edgeTable);
        makePolygonFace(negOffsetVertices, offsetMesh.faceTable, offsetMesh.edgeTable, true);
    }

    for(eIt = startMesh.edgeTable.begin(); eIt != startMesh.edgeTable.end(); eIt++) {
        Halfedge * currEdge = eIt -> second;
        if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
            vector<Vertex*> offsetSideVertices;
            if(currEdge -> start -> onMobiusSibling) {
                vec3 oneOption = currEdge -> start -> posOffset -> position - currEdge -> start -> position;
                if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> posOffset);
                } else {
                    offsetSideVertices.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> posOffset);
                }
            } else if(currEdge -> end -> onMobiusSibling) {
                vec3 oneOption = currEdge -> end -> posOffset -> position - currEdge -> end -> position;
                if(dot(oneOption, currEdge -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> posOffset);
                } else {
                    offsetSideVertices.push_back(currEdge -> start -> posOffset);
                    offsetSideVertices.push_back(currEdge -> start -> negOffset);
                    offsetSideVertices.push_back(currEdge -> end -> posOffset);
                    offsetSideVertices.push_back(currEdge -> end -> negOffset);
                }
            } else {
                offsetSideVertices.push_back(currEdge -> start -> posOffset);
                offsetSideVertices.push_back(currEdge -> start -> negOffset);
                offsetSideVertices.push_back(currEdge -> end -> negOffset);
                offsetSideVertices.push_back(currEdge -> end -> posOffset);
            }
            makePolygonFace(offsetSideVertices, offsetMesh.faceTable, offsetMesh.edgeTable);
        }
    }
    buildConnections(offsetMesh);
    computeNormals(offsetMesh);

}

void Offset::calcVertexOffset(Vertex * v, bool full) {
    Vertex * posOffset = new Vertex;
    Vertex * negOffset = new Vertex;
    posOffset -> position = v -> position + v -> normal * offsetVal;
    negOffset -> position = v -> position - v -> normal * offsetVal;
    posOffset -> ID = v -> ID + 100000;
    negOffset -> ID = v -> ID + 200000;
    posOffset -> normal = v -> normal;
    negOffset -> normal = - v -> normal;
    v -> posOffset = posOffset;
    v -> negOffset = negOffset;
    if(!full) {
        posOffset -> addToHashTable(posOffsetMesh.vertTable);
        negOffset -> addToHashTable(negOffsetMesh.vertTable);
    } else {
        posOffset -> addToHashTable(offsetMesh.vertTable);
        negOffset -> addToHashTable(offsetMesh.vertTable);
    }
}

#endif // __OFFSET_H__
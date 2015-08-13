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
    // The positive offset mesh.
    Mesh posOffsetMesh;
    // The negtive offset mesh.
    Mesh negOffsetMesh;
    // The side offset mesh.
    Mesh sideOffsetMesh;
    // Constructor of offset.
    // @param mesh: the initial mesh to find offset
    // @param val: the offsetVal
    Offset(Mesh & mesh, float val);
    // Change the value of offset
    void changeOffsetValTo(float val);

private:
    // Calculate the offset for one vertex.
    void calcVertexOffset(Vertex * v);
};

Offset::Offset(Mesh & mesh, float val) {
    offsetVal = val;
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    unordered_map<uint, Face*>::iterator fIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(vIt = mesh.vertTable.begin(); vIt != mesh.vertTable.end(); vIt++) {
        calcVertexOffset(vIt -> second);
    }

    for(fIt = mesh.faceTable.begin(); fIt != mesh.faceTable.end(); fIt++) {
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
    for(eIt = mesh.edgeTable.begin(); eIt != mesh.edgeTable.end(); eIt++) {
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
            makePolygonFace(offsetSideVertices, sideOffsetMesh.faceTable, sideOffsetMesh.edgeTable);
        }

    }
    buildConnections(sideOffsetMesh);
    computeNormals(sideOffsetMesh);
    
    // To build a complete offset mesh;
    buildConnections(posOffsetMesh);
    buildConnections(negOffsetMesh);
    computeNormals(posOffsetMesh);
    computeNormals(negOffsetMesh);
}
/*
void Offset::changeOffsetValTo(float val) {
    offsetVal = val;
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    for(vIt = startMesh.vertTable.begin(); 
        vIt < startMesh.vertTable.end(); vIt++) {
        (*vIt) -> posOffset -> position = (*vIt) -> position + (*vIt) -> normal * offsetVal;
        (*vIt) -> negOffset -> position = (*vIt) -> position - (*vIt) -> normal * offsetVal;
    }
}
*/
void Offset::calcVertexOffset(Vertex * v) {
    Vertex * posOffset = new Vertex;
    Vertex * negOffset = new Vertex;
    posOffset -> position = v -> position + v -> normal * offsetVal;
    negOffset -> position = v -> position - v -> normal * offsetVal;
    posOffset -> ID = v -> ID;
    negOffset -> ID = v -> ID;
//    posOffset -> normal = v -> normal;
//    negOffset -> normal = - v -> normal;
    v -> posOffset = posOffset;
    v -> negOffset = negOffset;
    posOffset -> addToHashTable(posOffsetMesh.vertTable);
    negOffset -> addToHashTable(negOffsetMesh.vertTable);
}

#endif // __OFFSET_H__
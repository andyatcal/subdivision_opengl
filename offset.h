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
    //void resetOffsetVal(float val);

private:
    // Calculate the offset for one vertex.
    void calcVertexOffset(Vertex * v);
};

Offset::Offset(Mesh & mesh, float val) {
    offsetVal = val;
    vector<Vertex*>::iterator vIt;
    vector<Face*>::iterator fIt;
    vector<Halfedge*>::iterator eIt;
    for(vIt = mesh.vertVect.begin(); vIt < mesh.vertVect.end(); vIt++) {
        calcVertexOffset(*vIt);
    }
    for(fIt = mesh.faceVect.begin(); fIt < mesh.faceVect.end(); fIt++) {
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        for(vIt = ((*fIt) -> vertices).begin(); vIt < ((*fIt) -> vertices).end(); vIt++) {
            if(!((*vIt) -> onMobiusSibling)) {
                posOffsetVertices.push_back((*vIt) -> posOffset);
                negOffsetVertices.push_back((*vIt) -> negOffset);
            } else {
                vec3 oneOption = (*vIt) -> posOffset -> position - (*vIt) -> position;
                if(dot(oneOption, (*fIt) -> faceNormal) >= 0) {
                    (*vIt) -> negOffset -> normal = - (*vIt) -> normal;
                    (*vIt) -> posOffset -> normal = (*vIt) -> normal;
                    posOffsetVertices.push_back((*vIt) -> posOffset);
                    negOffsetVertices.push_back((*vIt) -> negOffset);
                } else {
                    (*vIt) -> negOffset -> normal = (*vIt) -> normal;
                    (*vIt) -> posOffset -> normal = - (*vIt) -> normal;
                    posOffsetVertices.push_back((*vIt) -> negOffset);
                    negOffsetVertices.push_back((*vIt) -> posOffset);
                }
            }
        }
        makePolygonFace(posOffsetVertices, posOffsetMesh.faceVect, posOffsetMesh.edgeVect);
        makePolygonFace(negOffsetVertices, negOffsetMesh.faceVect, negOffsetMesh.edgeVect);
    }
    for(eIt = mesh.edgeVect.begin(); eIt < mesh.edgeVect.end(); eIt++) {
        if((*eIt) -> sibling == NULL && (*eIt) -> mobiusSibling == NULL) {
            vector<Vertex*> offsetSideVertices;
            if((*eIt) -> start -> onMobiusSibling) {
                vec3 oneOption = (*eIt) -> start -> posOffset -> position - (*eIt) -> start -> position;
                if(dot(oneOption, (*eIt) -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices.push_back((*eIt) -> start -> posOffset);
                    offsetSideVertices.push_back((*eIt) -> start -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> posOffset);
                } else {
                    offsetSideVertices.push_back((*eIt) -> start -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> start -> posOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> posOffset);
                }
            } else if((*eIt) -> end -> onMobiusSibling) {
                vec3 oneOption = (*eIt) -> end -> posOffset -> position - (*eIt) -> end -> position;
                if(dot(oneOption, (*eIt) -> heFace -> faceNormal) >= 0) {
                    offsetSideVertices.push_back((*eIt) -> start -> posOffset);
                    offsetSideVertices.push_back((*eIt) -> start -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> posOffset);
                } else {
                    offsetSideVertices.push_back((*eIt) -> start -> posOffset);
                    offsetSideVertices.push_back((*eIt) -> start -> negOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> posOffset);
                    offsetSideVertices.push_back((*eIt) -> end -> negOffset);
                }
            } else {
                offsetSideVertices.push_back((*eIt) -> start -> posOffset);
                offsetSideVertices.push_back((*eIt) -> start -> negOffset);
                offsetSideVertices.push_back((*eIt) -> end -> negOffset);
                offsetSideVertices.push_back((*eIt) -> end -> posOffset);
            }
            makePolygonFace(offsetSideVertices, sideOffsetMesh.faceVect, sideOffsetMesh.edgeVect);
        }

    }
    buildConnections(sideOffsetMesh);
    computeNormals(sideOffsetMesh);
    //buildConnections(posOffsetMesh);
    //buildConnections(negOffsetMesh);
    //computeNormals(posOffsetMesh);
    //computeNormals(negOffsetMesh);
}
/*
void Offset::resetOffsetVal(float val) {
    offsetVal = val;
    vector<Vertex*>::iterator vIt;
    for(vIt = (*eIt) -> startMesh.vertVect.begin(); vIt < (*eIt) -> startMesh.vertVect.(*eIt) -> end(); vIt++) {
        calcVertexOffset(*vIt);
    }
}
*/
void Offset::calcVertexOffset(Vertex * v) {
    Vertex * posOffset = new Vertex;
    Vertex * negOffset = new Vertex;
    posOffset -> position = v -> position + v -> normal * offsetVal;
    negOffset -> position = v -> position - v -> normal * offsetVal;
    posOffset -> normal = v -> normal;
    negOffset -> normal = - v -> normal;
    v -> posOffset = posOffset;
    v -> negOffset = negOffset;
    posOffsetMesh.vertVect.push_back(posOffset);
    negOffsetMesh.vertVect.push_back(negOffset);
}

#endif // __OFFSET_H__
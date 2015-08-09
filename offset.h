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
    // Starting mesh;
    Mesh startMesh;
    // Constructor of offset.
    // @param mesh: the initial mesh to find offset
    // @param val: the offsetVal
    Offset(Mesh & mesh, float val);
    void resetOffsetVal(float val);

private:
    // Calculate the offset for one vertex.
    void calcVertexOffset(Vertex * v);
};

Offset::Offset(Mesh & mesh, float val) {
    offsetVal = val;
    startMesh = mesh;
    vector<Vertex*>::iterator vIt;
    vector<Face*>::iterator fIt;
    Face * currFace;
    for(vIt = mesh.vertVect.begin(); vIt < mesh.vertVect.end(); vIt++) {
        calcVertexOffset(*vIt);
    }
    for(fIt = mesh.faceVect.begin(); fIt < mesh.faceVect.end(); fIt++) {
        currFace = (*fIt);
        vector<Vertex*> posOffsetVertices;
        vector<Vertex*> negOffsetVertices;
        for(vIt = ((*fIt) -> vertices).begin(); vIt < ((*fIt) -> vertices).end(); vIt++) {
            posOffsetVertices.push_back((*vIt) -> posOffset);
            negOffsetVertices.push_back((*vIt) -> negOffset);
        }
        makePolygonFace(posOffsetVertices, posOffsetMesh.faceVect, posOffsetMesh.edgeVect);
        makePolygonFace(negOffsetVertices, negOffsetMesh.faceVect, negOffsetMesh.edgeVect);
    }
    buildConnections(posOffsetMesh);
    buildConnections(negOffsetMesh);
    computeNormals(posOffsetMesh);
    computeNormals(negOffsetMesh);
}

void Offset::resetOffsetVal(float val) {
    offsetVal = val;
    vector<Vertex*>::iterator vIt;
    for(vIt = startMesh.vertVect.begin(); vIt < startMesh.vertVect.end(); vIt++) {
        calcVertexOffset(*vIt);
    }
}
void Offset::calcVertexOffset(Vertex * v) {
    Vertex * posOffset = new Vertex;
    Vertex * negOffset = new Vertex;
    posOffset -> position = v -> position + v -> normal * offsetVal;
    negOffset -> position = v -> position - v -> normal * offsetVal;
    v -> posOffset = posOffset;
    v -> negOffset = negOffset;
    posOffsetMesh.vertVect.push_back(posOffset);
    negOffsetMesh.vertVect.push_back(negOffset);
}

#endif // __OFFSET_H__
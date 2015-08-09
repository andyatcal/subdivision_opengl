// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <glm/glm.hpp>

using namespace glm;

// Forward declarations
class Halfedge;

//////////////////////////////////////////////////////////////////////
// Vertex Class -- 3D Vertex for Catmull-Clark Subdivision.
//
class Vertex{
public:
    // The position of this vertex.
    vec3 position;
    // One out-going halfedge of this vertex.
    Halfedge * oneOutEdge;
    // The normal of this Vertex.
    vec3 normal;
    // An ID to track this Vertex.
    int ID;
    // Track if this vertex on a mobius junction.
    bool onMobiusSibling;
    // Copy of vertex position, to avoid conficts in makeVertPoints of subdivision.
    vec3 copy;
    // Pointer to the positive offset for this vertex.
    Vertex * posOffset;
    // Pointer to the negative offset for this vertex.
    Vertex * negOffset;
    // Constructor.
    Vertex();
    // Contructor given initial location of vertex.
    // @param x, y, z as the initial x, y, z position of this vertex.
    Vertex(float x, float y, float z);
    // Set copy to the current position of this vertex.
    void makeCopy();
};

Vertex::Vertex(){
    position = vec3(0, 0, 0);
    oneOutEdge = NULL;
    ID = 0;
    onMobiusSibling = false;
    posOffset = negOffset = NULL;
}

Vertex::Vertex(float x, float y, float z) {
    position = vec3(x, y, z);
    oneOutEdge = NULL;
    ID = 0;
    onMobiusSibling = false;
    posOffset = negOffset = NULL;
}

void Vertex::makeCopy() {
    copy = position;
}

#endif // __VERTEX_H__
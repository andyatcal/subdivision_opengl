// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __FACE_H__
#define __FACE_H__

#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;

//////////////////////////////////////////////////////////////////////
// Face Class -- Polygon Face for Catmull-Clark Subdivision.
//
class Face{
public:
    // An arraylist of vertices that make this face.
    vector<Vertex*> vertices;
    // A pointer to one halfedge that construct this face.
    Halfedge * oneSideEdge;
    // A pointer to the face point after makeFacePoints in subdivision.
    Vertex * facePoint;
    // Constructor.
    Face();
    // Add one more vertex to this polygon face
    void addVertex(Vertex * v);
    // Tracking ID.
    int ID;
    // The normal of face, by Newell's method.
    vec3 faceNormal;
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

#endif // __FACE_H__
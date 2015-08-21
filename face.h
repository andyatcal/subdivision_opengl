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
    // A pointer to one halfedge that construct this face.
    Halfedge * oneSideEdge;
    // A pointer to the face point after makeFacePoints in subdivision.
    Vertex * facePoint;
    // Constructor.
    Face();
    // The normal of face, by Newell's method.
    vec3 faceNormal;
};

Face::Face(){
    oneSideEdge = NULL;
    facePoint = NULL;
}

#endif // __FACE_H__
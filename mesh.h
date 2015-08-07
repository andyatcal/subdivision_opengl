// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

using namespace std;

// Forward declarations
class Vertex;
class Face;
class Halfedge;

//////////////////////////////////////////////////////////////////////
// Mesh Class -- A MESH for display or subdivide.
class Mesh{
public:
    // An arraylist of all vertices that construct this mesh.
    vector<Vertex*> vertVect;
    // An arraylist of all halfedges that construct this mesh.
    vector<Halfedge*> edgeVect;
    // An arraylist of all faces that construct this mesh.
    vector<Face*> faceVect;
    // Constructor.
    Mesh();
};

Mesh::Mesh(){}

#endif // __MESH_H__
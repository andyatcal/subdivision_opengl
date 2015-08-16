// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <unordered_map>

using namespace std;

// Forward declarations
class Vertex;
class Face;
class Halfedge;

//////////////////////////////////////////////////////////////////////
// Mesh Class -- A MESH for display or subdivide.
class Mesh{
public:
    // An Hashtable of all halfedges.
    // Key = {Vertex ID}, Value = {pointer to the vertex}
    unordered_map<unsigned long, Vertex*> vertTable;
    // An Hashtable of all halfedges. 
    // Key = {Edge ID}, Value = {pointer to the edge}
    unordered_map<unsigned long long, Halfedge*> edgeTable;
    // An Hashtable of all halfedges. 
    // Key = {Face ID}, Value = {pointer to the face}
    unordered_map<uint, Face*> faceTable;
    // Transfromation matrix of the object (mesh).
    mat4 object2world;
    // Constructor.
    Mesh();
};

Mesh::Mesh(){
	object2world = mat4(1);
}

#endif // __MESH_H__
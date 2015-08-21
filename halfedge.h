// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __HALFEDGE_H__
#define __HALFEDGE_H__

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;

//////////////////////////////////////////////////////////////////////
// Halfedge Class -- Halfedge, Vertex A -> Vertex B,  for Catmull-Clark Subdivision.
//
class Halfedge{
public:
	// Pointer to the start of the edge.
    Vertex * start;
    // Pointer to the end of the edge.
    Vertex * end;
    // Pointer to the normal sibling of the edge.
    // Example: A -> B and B -> A is a pair of normal sibling.
    Halfedge * sibling;
    // Pointer to the mobius sibling of the edge.
    // Example: A -> B and another A -> B is a pair of mobius sibling.
    Halfedge * mobiusSibling;
    // The next halfedge to contruct the face.
    // For mobius feature.
    Halfedge * next;
    // The previous halfedge to construct the face.
    Halfedge * previous;
    // Face that this halfedge construct.
    Face * heFace;
    // A pointer to the edge point after makeEdgePoints in subdivision.
    Vertex * edgePoint;
    // The first part of this edge after division into two parts at the edgePoint.
    Halfedge * firstHalf;
    // The second part of this edge after division into two parts at the edgePoint.
    Halfedge * secondHalf;
    // True if this edge is sharp in subdivision.
    // For boundary and sharp edge feature.
    bool isSharp;
    // The next boudary edge immediately connected to this edge.
    // For boundary feature.
    Halfedge * nextBoundary;
    // The previous boundary edge immediately connected to this edge.
    // For boundary feature.
    Halfedge * previousBoundary;
    // The mobius boundary edge immediately connected to this edge.
    // For boundary and mobius feature.
    Halfedge * mobiusBoundary;
    // Constructor.
    Halfedge();
    // Constructor with start and end vertex.
    // @param v1, v2: pointers to the start and end vertices.
    Halfedge(Vertex * v1, Vertex * v2);
    // Add this edge to a hashTable.
    // @param edgeTable: the target HashTable to be added in.
    void addToHashTable(unordered_map<unsigned long long, Halfedge*> & edgeTable);
};

Halfedge::Halfedge() {
    start = end = NULL;
    sibling = NULL;
    next = previous = NULL;
    heFace = NULL;
    edgePoint = NULL;
    firstHalf = secondHalf = NULL;
    isSharp = false;
    nextBoundary = NULL;
    previousBoundary = NULL;
    mobiusSibling = NULL;
    mobiusBoundary = NULL;
    sibling = mobiusSibling = NULL;
}

Halfedge::Halfedge(Vertex * v1, Vertex * v2) {
    start = v1;
    start -> oneOutEdge = this;
    end = v2;
    sibling = NULL;
    next = previous = NULL;
    heFace = NULL;
    edgePoint = NULL;
    firstHalf = secondHalf = NULL;
    isSharp = false;
    nextBoundary = NULL;
    previousBoundary = NULL;
    mobiusSibling = NULL;
    mobiusBoundary = NULL;
    sibling = mobiusSibling = NULL;
}

#endif // __HALFEDGE_H__
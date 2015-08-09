// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __MESHUTILS_H__
#define __MESHUTILS_H__

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;
class Halfedge;
class Mesh;

//////////////////////////////////////////////////////////////////////
// Subdivision Class -- Functions to perform the subdivision for a mesh

// Flush mesh. Make the vertVect, edgeVect, faceVect of a mesh empty.
// @param mesh : the mesh to flush.
void flushMesh(Mesh &mesh) {
    vector<Face*> faceVect = mesh.faceVect;
    vector<Halfedge*> edgeVect = mesh.edgeVect;
    vector<Vertex*> vertVect = mesh.vertVect;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;
    while(!faceVect.empty()){
        tempFace = faceVect.back();
        faceVect.pop_back();
        delete tempFace;
    }
    while(!edgeVect.empty()){
        tempEdge = edgeVect.back();
        edgeVect.pop_back();
        delete tempEdge;
    }
    while(!vertVect.empty()){
        tempVert = vertVect.back();
        vertVect.pop_back();
        delete tempVert;
    }
}

// Make a triangle face with three vertex.
// @param va, vb, vc: three points to construct the face.
// @param faceVect: reference to the faceVect of the mesh.
// @param edgeVect: reference to the edgeVect of the mesh.
void makeTriFace(Vertex * va, Vertex* vb, Vertex * vc, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect) {
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e1->end = vb;
    e2->end = vc;
    e3->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;

    e1->next = e2;
    e2->next = e3;
    e3->next = e1;

    e2->previous = e1;
    e3->previous = e2;
    e1->previous = e3;
    
    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;

    nextFace->oneSideEdge = e3;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
}

// Make a rectangle face with four vertex.
// @param va, vb, vc, vd: four points to construct the face.
// @param faceVect: reference to the faceVect of the mesh.
// @param edgeVect: reference to the edgeVect of the mesh.
void makeRectFace(Vertex * va, Vertex* vb, Vertex * vc, Vertex * vd, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect) {
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3, *e4;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;
    e4 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);
    nextFace -> addVertex(vd);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e4->start = vd;
    e1->end = vb;
    e2->end = vc;
    e3->end = vd;
    e4->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;
    vd->oneOutEdge = e4;

    e1->next = e2;
    e2->next = e3;
    e3->next = e4;
    e4->next = e1;

    e2->previous = e1;
    e3->previous = e2;
    e4->previous = e3;
    e1->previous = e4;

    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;
    e4->heFace = nextFace;

    nextFace->oneSideEdge = e4;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
    edgeVect.push_back(e4);
}

// Make a polygon face with n vertex.
// @param vertices: a list of points to construct the face.
// @param faceVect: reference to the faceVect of the mesh.
// @param edgeVect: reference to the edgeVect of the mesh.
void makePolygonFace(vector<Vertex*> vertices, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect) {
    Face * nextFace = new Face;
    vector<Halfedge*> edges;
    vector<Vertex*>::iterator vIt;
    Vertex * currVert;
    Halfedge * currEdge;
    for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
        currVert = *vIt;
        Halfedge * tempEdge = new Halfedge;
        tempEdge -> start = *vIt;
        if(vIt == (vertices.end() - 1)) {
            tempEdge -> end = *(vertices.begin());
        } else {
            tempEdge -> end = *(vIt + 1);
        }
        edges.push_back(tempEdge);
        currVert -> oneOutEdge = tempEdge;
        nextFace -> addVertex(*vIt);
    }
    
    vector<Halfedge*>::iterator eIt;
    for(eIt = edges.begin(); eIt < edges.end(); eIt++) {
        currEdge = *eIt;
        if(eIt == (edges.end() - 1)) {
            currEdge -> next = *(edges.begin());
            Halfedge * temp = *(edges.begin());
            temp -> previous = currEdge;
        } else {
            currEdge -> next = *(eIt + 1);
            Halfedge * temp = *(eIt + 1);
            temp -> previous = currEdge;
        }
        currEdge -> heFace = nextFace;
        edgeVect.push_back(*eIt);
    }
    nextFace -> oneSideEdge = *(edges.begin());
    faceVect.push_back(nextFace);
}

// Build connnections after making the faces.
// @param mesh: reference to the mesh that the connections located in.
void buildConnections(Mesh &mesh) {
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    vector<Halfedge*>::iterator eIt;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 + 1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) && ((*edgeIt1)->end
             == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            } else if (((*edgeIt1) -> start == (*edgeIt2) -> start)
             &&((*edgeIt1) -> end == (*edgeIt2) -> end)) {

                (*edgeIt1)->mobiusSibling = *edgeIt2;
                (*edgeIt2)->mobiusSibling = *edgeIt1;

                (*edgeIt1) -> start -> onMobiusSibling = true;
                (*edgeIt1) -> end -> onMobiusSibling = true;
                (*edgeIt2) -> start -> onMobiusSibling = true;
                (*edgeIt2) -> end -> onMobiusSibling = true;

            }
        }
    }
    vector<Halfedge*> boundaryEdges;
    for(eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++) {
        if((*eIt) -> sibling == NULL && (*eIt) -> mobiusSibling == NULL) {
            boundaryEdges.push_back(*eIt);
        }
    }
    for( edgeIt1 = boundaryEdges.begin(); edgeIt1 < boundaryEdges.end();
     edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < boundaryEdges.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->start) &&((*edgeIt1)->end
             != (*edgeIt2)->end)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if (((*edgeIt1)->end == (*edgeIt2)->end) &&((*edgeIt1)->start
             != (*edgeIt2)->start)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if ((*edgeIt1)->start == (*edgeIt2)->end){

                (*edgeIt1)->previousBoundary = *edgeIt2;
                (*edgeIt2)->nextBoundary = *edgeIt1;

            } else if ((*edgeIt1)->end == (*edgeIt2)->start){

                (*edgeIt1)->nextBoundary = *edgeIt2;
                (*edgeIt2)->previousBoundary = *edgeIt1;

            }
        }
    }
    for(eIt = boundaryEdges.begin(); eIt < boundaryEdges.end(); eIt ++) {
        (*eIt) -> isSharp = true;
    }
}

// Make the boundaries given the vertices on the boundaries.
// @param boundaries: lists of vertices on all boundaries.
// @param edgeVect: reference to the edgeVect of the mesh.
void makeBoundaries(vector<vector<Vertex*> > &boundaries,
 vector<Halfedge*> &edgeVect) {
    vector<vector<Vertex*> >::iterator allBoundsIt;
    vector<Vertex*> oneBoundary;
    vector<Halfedge*> boundaryEdges;
    vector<Halfedge*>::iterator eIt;
    for(allBoundsIt = boundaries.begin(); allBoundsIt < boundaries.end();
     allBoundsIt++) {
        oneBoundary = *allBoundsIt;
        vector<Vertex*>::iterator boundIt;
        for(boundIt = oneBoundary.begin(); boundIt < oneBoundary.end();
         boundIt ++) {
            for(eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt++) {
                if(boundIt != oneBoundary.end() - 1 && (*eIt) -> start ==
                 (*boundIt) && (*eIt) -> end == *(boundIt + 1)){
                    boundaryEdges.push_back(*eIt);
                } else if(boundIt == oneBoundary.end() - 1 && (*eIt) -> start ==
                 (*boundIt) && (*eIt) -> end == *(oneBoundary.begin())){
                    boundaryEdges.push_back(*eIt);
                }
            }
        }
    }
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = boundaryEdges.begin(); edgeIt1 < boundaryEdges.end();
     edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < boundaryEdges.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->start) &&((*edgeIt1)->end
             != (*edgeIt2)->end)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if (((*edgeIt1)->end == (*edgeIt2)->end) &&((*edgeIt1)->start
             != (*edgeIt2)->start)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if ((*edgeIt1)->start == (*edgeIt2)->end){

                (*edgeIt1)->previousBoundary = *edgeIt2;
                (*edgeIt2)->nextBoundary = *edgeIt1;

            } else if ((*edgeIt1)->end == (*edgeIt2)->start){

                (*edgeIt1)->nextBoundary = *edgeIt2;
                (*edgeIt2)->previousBoundary = *edgeIt1;

            }
        }
    }
    for(eIt = boundaryEdges.begin(); eIt < boundaryEdges.end(); eIt ++) {

        if((*eIt) -> mobiusSibling == NULL) {
            (*eIt) -> isSharp = true;
        }
    }
}

// Get the vertex normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
vec3 getNormalEndOfEdge(Halfedge * currEdge){
    Vertex * v1 = currEdge -> start;
    Vertex * v2 = currEdge -> end;
    Vertex * v3 = currEdge -> next -> end;

    vec3 oneEdge = v2 -> position - v1 -> position;
    vec3 secondEdge = v3 -> position - v2 -> position;

    vec3 result = cross(normalize(oneEdge), normalize(secondEdge));

    return normalize(result);
}

vec3 getNormalStartOfEdge(Halfedge * currEdge){
    Vertex * v1 = currEdge -> previous -> start;
    Vertex * v2 = currEdge -> start;
    Vertex * v3 = currEdge -> end;

    vec3 oneEdge = v2 -> position - v1 -> position;
    vec3 secondEdge = v3 -> position - v2 -> position;

    vec3 result = cross(normalize(oneEdge), normalize(secondEdge));

    return normalize(result);
}

//iterate over every vertex in the mesh and compute its normal
// @param vertVect: the vector of vertex to compute normals.
void computeNormals(vector<Vertex*> &vertVect){

    vector<Vertex*>::iterator it;
    Halfedge * firstOutEdge;
    Halfedge * nextOutEdge;
    Vertex * currVert;

    for(it = vertVect.begin(); it < vertVect.end(); it++){
        currVert = *it;
        int n = 0;
        vec3 avgNorm = vec3(0, 0, 0);
        firstOutEdge = currVert -> oneOutEdge;
        nextOutEdge = firstOutEdge;
        do {
            if(nextOutEdge -> sibling == NULL && nextOutEdge -> mobiusSibling == NULL) {
                avgNorm += getNormalEndOfEdge(nextOutEdge -> previousBoundary);
                nextOutEdge = nextOutEdge -> previousBoundary -> next;   
            } else {
                // This Part need to be fixed. Andy
                avgNorm += getNormalEndOfEdge(nextOutEdge -> sibling);
                nextOutEdge = nextOutEdge -> sibling -> next;
            }
            n += 1;
        } while ( nextOutEdge != firstOutEdge);
        avgNorm /= n;
        currVert->normal = normalize(avgNorm);
    }
}
#endif // __MESHUTILS_H__
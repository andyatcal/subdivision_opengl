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
// MeshUtils Class -- Utility Functions with Mesh.

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
                vector<Halfedge*> &edgeVect,
                bool reverseOrder = false) {
    Face * nextFace = new Face;
    vector<Halfedge*> edges;
    vector<Vertex*>::iterator vIt;
    Vertex * currVert;
    Halfedge * currEdge;
    if(!reverseOrder) {
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
        }
    } else {
        for(vIt = vertices.end() - 1; vIt >= vertices.begin(); vIt--) {
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
        }
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

// Return the normal at point v2. 
// @param p1, p2, p3 are positions of three vertices,
// with edge p1 -> p2 and edge p2 -> p3.
vec3 getNormal3Vertex(vec3 p1, vec3 p2, vec3 p3){
    return normalize(cross(p2 - p1, p3 - p2));
}

// Get the normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
vec3 getEndOfEdgeNormal(Halfedge * currEdge){
    Vertex * v1 = currEdge -> start;
    Vertex * v2 = currEdge -> end;
    Vertex * v3 = currEdge -> next -> end;
    return getNormal3Vertex(v1 -> position, v2 -> position, v3 -> position);
}

// Get the surface normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
void getFaceNormal(Face * currFace){
    Halfedge * firstEdge = currFace -> oneSideEdge;
    Halfedge * currEdge;
    currEdge = firstEdge;
    vec3 avgNorm(0, 0, 0);
    do {
        avgNorm += getEndOfEdgeNormal(currEdge);
        currEdge = currEdge -> next;
    } while (currEdge != firstEdge);
    currFace -> faceNormal = normalize(avgNorm);;
}

// Get the surface normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
void getVertexNormal(Vertex * currVert){
    Halfedge * firstOutEdge;
    Halfedge * nextOutEdge;
    firstOutEdge = currVert -> oneOutEdge;
    nextOutEdge = firstOutEdge;
    vec3 avgNorm(0, 0, 0);
    int mobiusCounter = 0;
    do {
        if(mobiusCounter % 2 == 0) {
            avgNorm += nextOutEdge -> heFace -> faceNormal;
        } else {
            avgNorm -= nextOutEdge -> heFace -> faceNormal;
        }
        if(nextOutEdge -> sibling != NULL) {
            if(mobiusCounter % 2 == 0) {
                nextOutEdge = nextOutEdge -> sibling -> next;
            } else {
                nextOutEdge = nextOutEdge -> sibling -> previous;
            }
        } else if(nextOutEdge -> mobiusSibling != NULL) {
            mobiusCounter += 1;
            if(mobiusCounter % 2 == 0) {
                nextOutEdge = nextOutEdge -> mobiusSibling -> next;
            } else {
                nextOutEdge = nextOutEdge -> mobiusSibling -> previous;
            }
        } else if(!(currVert -> onMobiusSibling)) {
            if(mobiusCounter % 2 == 0) {
                nextOutEdge = nextOutEdge -> previousBoundary -> next;
            } else {
                nextOutEdge = nextOutEdge -> previousBoundary -> previous;
            }
        } else {
            mobiusCounter += 1;
            if(mobiusCounter % 2 == 0) {
                nextOutEdge = nextOutEdge -> mobiusBoundary -> next;
            } else {
                nextOutEdge = nextOutEdge -> mobiusBoundary -> previous;
            }
        }
    } while ( nextOutEdge != firstOutEdge);
    currVert -> normal = normalize(avgNorm);
}

// Iterate over every vertex in the mesh and compute its normal
// @param vertVect: the vector of vertex to compute normals.
void computeNormals(Mesh & mesh){
    vector<Vertex*> vertVect = mesh.vertVect;
    vector<Vertex*>::iterator vIt;
    vector<Face*> faceVect = mesh.faceVect;
    vector<Face*>::iterator fIt;
    for(fIt = faceVect.begin(); fIt < faceVect.end(); fIt++){
        getFaceNormal(*fIt);
    }
    for(vIt = vertVect.begin(); vIt < vertVect.end(); vIt++){
        getVertexNormal(*vIt);
    }
}

// Draw a mesh in OpenGL with GL_POLYGON
// @param mesh: reference to the mesh to draw.
void drawMesh(Mesh & mesh) {
    Face * tempFace;
    vector<Face*>::iterator fIt;
    //cout<<endl;
    for(fIt = mesh.faceVect.begin(); fIt < mesh.faceVect.end(); fIt++){
        //cout<<"A new Face Begin HERE!"<<endl;
        tempFace = *fIt;
        Vertex * tempv;
        Halfedge * firstSideEdge = (*fIt) -> oneSideEdge;
        if(firstSideEdge == NULL) {
            cout<<"ERROR: This face (with ID)" <<(*fIt) -> ID << "does not have a sideEdge."<<endl;
            exit(1);
        }
        glBegin(GL_POLYGON);
        Halfedge * nextSideEdge = firstSideEdge;
        do {
            tempv = nextSideEdge -> end;
            float normx = tempv -> normal[0];
            float normy = tempv -> normal[1];
            float normz = tempv -> normal[2];
            //cout<<"normx: "<<normx<<" normy: "<<normy<<" normz: "<<normz<<endl;
            glNormal3f(normx, normy, normz);
            float x = tempv -> position[0];
            float y = tempv -> position[1];
            float z = tempv -> position[2];
            //cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
            glVertex3f(x, y, z);
            nextSideEdge = nextSideEdge -> next;
        } while(nextSideEdge != firstSideEdge);
        glEnd();
    }
}
#endif // __MESHUTILS_H__
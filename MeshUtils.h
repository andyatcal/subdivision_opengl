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

// Flush mesh. Make the vertTable, edgeTable, faceTable of a mesh empty.
// @param mesh : the mesh to flush.
void flushMesh(Mesh &mesh) {
    unordered_map<uint, Face*> faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> edgeTable = mesh.edgeTable;
    unordered_map<unsigned long, Vertex*> vertTable = mesh.vertTable;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;
    unordered_map<uint, Face*>::iterator fIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    for(fIt = faceTable.begin(); fIt != faceTable.end(); fIt++) {
        tempFace = fIt -> second;
        delete tempFace;
    }
    for(eIt = edgeTable.begin(); eIt != edgeTable.end(); eIt++) {
        tempEdge = eIt -> second;
        delete tempEdge;
    }
    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++) {
        tempVert = vIt -> second;
        delete tempVert;
    }
    faceTable.clear();
    edgeTable.clear();
    vertTable.clear();
}

// Make a triangle face with three vertex.
// @param va, vb, vc: three points to construct the face.
// @param faceTable: reference to the faceTable of the mesh.
// @param edgeTable: reference to the edgeTable of the mesh.
void makeTriFace(Vertex * va, Vertex* vb, Vertex * vc, 
                unordered_map<uint, Face*> &faceTable, 
                unordered_map<unsigned long long, Halfedge*> &edgeTable) {
    Face * currFace = new Face;
    Halfedge *e1, *e2, *e3;

    e1 = new Halfedge(va, vb);
    e2 = new Halfedge(vb, vc);
    e3 = new Halfedge(vc, va);

    e1 -> next = e2;
    e2 -> next = e3;
    e3 -> next = e1;

    e2 -> previous = e1;
    e3 -> previous = e2;
    e1 -> previous = e3;
    
    e1 -> heFace = currFace;
    e2 -> heFace = currFace;
    e3 -> heFace = currFace;

    currFace -> oneSideEdge = e3;
    currFace -> ID = faceTable.size();
    currFace -> addToHashTable(faceTable);

    e1 -> addToHashTable(edgeTable);
    e2 -> addToHashTable(edgeTable);
    e3 -> addToHashTable(edgeTable);

}


// Make a rectangle face with four vertex.
// @param va, vb, vc, vd: four points to construct the face.
// @param faceTable: reference to the faceTable of the mesh.
// @param edgeTable: reference to the edgeTable of the mesh.
void makeRectFace(Vertex * va, Vertex* vb, Vertex * vc, Vertex * vd, 
                unordered_map<uint, Face*> &faceTable, 
                unordered_map<unsigned long long, Halfedge*> &edgeTable) {
    Face * currFace = new Face;
    Halfedge *e1, *e2, *e3, *e4;

    e1 = new Halfedge(va, vb);
    e2 = new Halfedge(vb, vc);
    e3 = new Halfedge(vc, vd);
    e4 = new Halfedge(vd, va);

    e1 -> next = e2;
    e2 -> next = e3;
    e3 -> next = e4;
    e4 -> next = e1;

    e2 -> previous = e1;
    e3 -> previous = e2;
    e4 -> previous = e3;
    e1 -> previous = e4;

    e1 -> heFace = currFace;
    e2 -> heFace = currFace;
    e3 -> heFace = currFace;
    e4 -> heFace = currFace;

    currFace->oneSideEdge = e4;
    currFace -> ID = faceTable.size();
    currFace -> addToHashTable(faceTable);

    e1 -> addToHashTable(edgeTable);
    e2 -> addToHashTable(edgeTable);
    e3 -> addToHashTable(edgeTable);
    e4 -> addToHashTable(edgeTable);
}

// Make a polygon face with n vertex.
// @param vertices: a list of points to construct the face.
// @param faceTable: reference to the faceTable of the mesh.
// @param edgeTable: reference to the edgeTable of the mesh.
void makePolygonFace(vector<Vertex*> vertices, 
                unordered_map<uint, Face*> &faceTable, 
                unordered_map<unsigned long long, Halfedge*> &edgeTable,
                bool reverseOrder = false) {
    Face * currFace = new Face;
    vector<Halfedge*> edges;
    vector<Vertex*>::iterator vIt;
    Vertex * currVert;
    Halfedge * currEdge;
    if(!reverseOrder) {
        for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
            currVert = *vIt;
            Halfedge * tempEdge;
            if(vIt == (vertices.end() - 1)) {
                tempEdge = new Halfedge(*vIt, *(vertices.begin()));
            } else {
                tempEdge = new Halfedge(*vIt, *(vIt + 1));
            }
            edges.push_back(tempEdge);
        }
    } else {
        for(vIt = vertices.end() - 1; vIt >= vertices.begin(); vIt--) {
            currVert = *vIt;
            Halfedge * tempEdge;
            if(vIt == (vertices.end() - 1)) {
                tempEdge = new Halfedge(*(vertices.begin()), *vIt);
            } else {
                tempEdge = new Halfedge(*(vIt + 1), *vIt);
            }
            edges.push_back(tempEdge);
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
        currEdge -> heFace = currFace;
        currEdge -> addToHashTable(edgeTable);
    }
    currFace -> oneSideEdge = *(edges.begin());
    currFace -> ID = faceTable.size();
    currFace -> addToHashTable(faceTable);
}

// Given the list of boundary edges, build the connections between halfedges.
// @param boundaryEdges. A list of edges on the boundary, not necessarily sorted.
// @param edgeTable: reference to the edgeTable of the mesh.
void buildBoundaries(vector<Halfedge*> & boundaryEdges) {
    vector<Halfedge*>::iterator eIt1;
    vector<Halfedge*>::iterator eIt2;
    for(eIt1 = boundaryEdges.begin(); eIt1 < boundaryEdges.end(); eIt1++) {
        Halfedge * e1 = *eIt1;
        e1 -> isSharp = true;
        for(eIt2 = eIt1 + 1; eIt2 < boundaryEdges.end(); eIt2++) {
            Halfedge * e2 = *eIt2;
            if(e1 -> start == e2 -> start) {
                e1 -> mobiusBoundary = e2;
                e2 -> mobiusBoundary = e1;
            } else if (e1 -> end == e2 -> end) {
                e1 -> mobiusBoundary = e2;
                e2 -> mobiusBoundary = e1;
            } else if (e1 -> start == e2 -> end) {
                e1 -> previousBoundary = e2;
                e2 -> nextBoundary = e1;
            } else if (e1 -> end == e2 -> start){
                e1 -> nextBoundary = e2;
                e2 -> previousBoundary = e1;
            }
        }
    }
}

// Build connnections after making the faces.
// @param mesh: reference to the mesh that the connections located in.
void buildConnections(Mesh &mesh) {
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    unordered_map<unsigned long long, Halfedge*>::iterator eiIt;
    for(eIt = edgeTable.begin(); eIt != edgeTable.end(); eIt ++){
        Halfedge * currEdge = eIt -> second;
        if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
            Vertex * start = currEdge -> start;
            Vertex * end = currEdge -> end;
            unsigned long searchKey = hashKey(end -> ID, start -> ID);
            eiIt = edgeTable.find(searchKey);
            if(eiIt != edgeTable.end()) {
                Halfedge * mySibling = eiIt -> second;
                currEdge -> sibling = mySibling;
                mySibling -> sibling = currEdge;
            }
        }
    }
    vector<Halfedge*> boundaryEdges;
    for(eIt = edgeTable.begin(); eIt != edgeTable.end(); eIt ++) {
        Halfedge * currEdge = eIt -> second;
        if(currEdge -> sibling == NULL && currEdge -> mobiusSibling == NULL) {
            boundaryEdges.push_back(currEdge);
        }
    }
    buildBoundaries(boundaryEdges);
}

// Return the normal at point v2. 
// @param p1, p2, p3 are positions of three vertices,
// with edge p1 -> p2 and edge p2 -> p3.
vec3 getNormal3Vertex(vec3 p1, vec3 p2, vec3 p3){
    /*cout<<"p1: "<<p1[0]<<" "<<p1[1]<<" "<<p1[2]<<endl;
    cout<<"p2: "<<p2[0]<<" "<<p2[1]<<" "<<p2[2]<<endl;
    cout<<"p3: "<<p3[0]<<" "<<p3[1]<<" "<<p3[2]<<endl;*/
    return normalize(cross(p2 - p1, p3 - p2));
}

// Get the normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
vec3 getEndOfEdgeNormal(Halfedge * currEdge){
    Vertex * v1 = currEdge -> start;
    Vertex * v2 = currEdge -> end;
    Vertex * v3 = currEdge -> next -> end;
    /*
    cout<<"ME ID: "<<currEdge -> ID<<endl;
    cout<<"ME: "<<currEdge -> start -> ID<<" "<< currEdge -> end -> ID<<" "<<endl;
    cout<<"NEXT: "<<currEdge -> next -> start -> ID<<" "<< currEdge -> next -> end -> ID<<" "<<endl;
    vec3 result = getNormal3Vertex(v1 -> position, v2 -> position, v3 -> position);
    */
    //cout<<"fnormx: "<<result[0]<<" fnormy: "<<result[1]<<" fnormz: "<<result[2]<<endl;
    return getNormal3Vertex(v1 -> position, v2 -> position, v3 -> position);
}

// Get the surface normal at the end of a halfedge. 
// @param currEdge: pointer of the edge, which the vertex on the end of.
void getFaceNormal(Face * currFace){
    //cout<<endl<<endl<<"New Face: "<<currFace -> ID <<endl;
    Halfedge * firstEdge = currFace -> oneSideEdge;
    Halfedge * currEdge;
    currEdge = firstEdge;
    vec3 avgNorm(0, 0, 0);
    do {
        avgNorm += getEndOfEdgeNormal(currEdge);
        //cout<<"fanormx: "<<avgNorm[0]<<" fanormy: "<<avgNorm[1]<<" fanormz: "<<avgNorm[2]<<endl;
        currEdge = currEdge -> next;
    } while (currEdge != firstEdge);
    vec3 result = normalize(avgNorm);
    //cout<<"fnormx: "<<avgNorm[0]<<" fnormy: "<<avgNorm[1]<<" fnormz: "<<avgNorm[2]<<endl;
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
        //cout<<"The value of avgNorm is :"<<avgNorm[0]<<" "<<avgNorm[1]<<" "<<avgNorm[2]<<endl;
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
    //cout<<"The value of avgNorm is :"<<avgNorm[0]<<" "<<avgNorm[1]<<" "<<avgNorm[2]<<endl;
    //cout<<"ID: "<<currVert -> ID <<" has "<<mobiusCounter<<" mConter"<<endl;
    currVert -> normal = normalize(avgNorm);
}

// Iterate over every vertex in the mesh and compute its normal
// @param vertTable: the vector of vertex to compute normals.
void computeNormals(Mesh & mesh){
    unordered_map<unsigned long, Vertex*> & vertTable = mesh.vertTable;
    unordered_map<unsigned long, Vertex*>::iterator vIt;
    unordered_map<uint, Face*> & faceTable = mesh.faceTable;
    unordered_map<uint, Face*>::iterator fIt;
    //cout<<"faceTable size: "<<faceTable.size()<<endl;
    for(fIt = faceTable.begin(); fIt != faceTable.end(); fIt++){
        getFaceNormal(fIt -> second);
    }
    //cout<<"the vertTable size is"<<vertTable.size()<<endl;
    for(vIt = vertTable.begin(); vIt != vertTable.end(); vIt++){
        getVertexNormal(vIt -> second);
    }
}

// Draw a mesh in OpenGL with GL_POLYGON
// @param mesh: reference to the mesh to draw.
void drawMesh(Mesh & mesh) {
    Face * tempFace;
    unordered_map<uint, Face*>::iterator fIt;
    //cout<<endl;
    for(fIt = mesh.faceTable.begin(); fIt != mesh.faceTable.end(); fIt++){
        //cout<<"A new Face Begin HERE!"<<endl;
        tempFace = fIt -> second;
        Vertex * tempv;
        Halfedge * firstSideEdge = fIt -> second -> oneSideEdge;
        //cout<<"New Face: "<<endl;
        if(firstSideEdge == NULL) {
            cout<<"ERROR: This face (with ID)" <<fIt -> second -> ID << "does not have a sideEdge."<<endl;
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
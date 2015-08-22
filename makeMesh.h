// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __MAKEMESH_H__
#define __MAKEMESH_H__

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;
class Halfedge;

//////////////////////////////////////////////////////////////////////
// MeshUtils Class -- Utility Functions with Mesh.
void makeSquare(Mesh &mesh) {
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;

    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addQuadFaceToMesh(v1, v2, v3, v4, mesh);
    buildConnections(mesh);

}

void makePyramid(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;

    v1 -> position = vec3(0, 0, 1);
    v2 -> position = vec3(1, 1, -1);
    v3 -> position = vec3(1, -1, -1);
    v4 -> position = vec3(-1, -1, -1);
    v5 -> position = vec3(-1, 1, -1);

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);

    vector<Vertex*> bottomFace;
    vector<Vertex*> bottomFaceR;
    bottomFace.push_back(v3);
    bottomFace.push_back(v2);
    bottomFace.push_back(v5);
    bottomFace.push_back(v4);

    bottomFaceR.push_back(v4);
    bottomFaceR.push_back(v5);
    bottomFaceR.push_back(v2);
    bottomFaceR.push_back(v3);

    //bottomFace
    addPolygonFaceToMesh(bottomFace, mesh);
    //topfrontFace
    addTriFaceToMesh(v1, v2, v3, mesh);
    //topbackFace
    addTriFaceToMesh(v1, v3, v4, mesh);
    //topleftFace
    addTriFaceToMesh(v1, v4, v5, mesh);
    //toprightFace
    addTriFaceToMesh(v1, v5, v2, mesh);

    buildConnections(mesh);
}

void makeCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);
 
    //topFace
    addQuadFaceToMesh(v1, v2, v3, v4, mesh);
    //bottomFace
    addQuadFaceToMesh(v6, v5, v8, v7, mesh);
    //leftFace
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    //rightFace
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);
    //frontFace
    addQuadFaceToMesh(v2, v1, v5, v6, mesh);
    //baceFace
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);

    buildConnections(mesh);
}

void makeOpenCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);

    //without the topFace
    //addQuadFaceToMesh(v1, v2, v3, v4, mesh);
    //bottomFace
    addQuadFaceToMesh(v6, v5, v8, v7, mesh);
    //leftFace
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    //rightFace
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);
    //frontFace
    addQuadFaceToMesh(v2, v1, v5, v6, mesh);
    //baceFace
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);

    //Boundaries
    buildConnections(mesh);
}

void makeRing(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);

    //leftFace
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    //rightFace
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);
    //frontFace
    addQuadFaceToMesh(v2, v1, v5, v6, mesh);
    //baceFace
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);

    buildConnections(mesh);
}

void makeSharpCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);

    //without the topFace
    addQuadFaceToMesh(v1, v2, v3, v4, mesh);
    //bottomFace
    addQuadFaceToMesh(v6, v5, v8, v7, mesh);
    //leftFace
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    //rightFace
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);
    //frontFace
    addQuadFaceToMesh(v2, v1, v5, v6, mesh);
    //baceFace
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);
    
    buildConnections(mesh);
    /*
    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(eIt = edgeTable.begin(); eIt != edgeTable.end(); eIt++) {
        if (eIt -> second -> start == v1 || eIt -> second -> end == v1) {
            eIt -> second -> isSharp = true;
        }

        else if(eIt -> second -> start == v2 && eIt -> second -> end == v1) {
            eIt -> second -> isSharp = true;
        } else if (eIt -> second -> start == v2 && eIt -> second -> end == v3) {
            eIt -> second -> isSharp = true;
        } else if(eIt -> second -> start == v3 && eIt -> second -> end == v2) {
            eIt -> second -> isSharp = true;
        } else if (eIt -> second -> start == v3 && eIt -> second -> end == v4) {
            eIt -> second -> isSharp = true;
        } else if(eIt -> second -> start == v4 && eIt -> second -> end == v3) {
            eIt -> second -> isSharp = true;
        } else if (eIt -> second -> start == v1 && eIt -> second -> end == v4) {
            eIt -> second -> isSharp = true;
        } else if(eIt -> second -> start == v4 && eIt -> second -> end == v1) {
            eIt -> second -> isSharp = true;
        }

    }
    */
}

void makeOctahedron(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
 
    v1 -> position = vec3(0, 0, 1.414);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.414);

    addTriFaceToMesh(v1, v2, v3, mesh);
    addTriFaceToMesh(v1, v3, v4, mesh);
    addTriFaceToMesh(v1, v4, v5, mesh);
    addTriFaceToMesh(v1, v5, v2, mesh);
    addTriFaceToMesh(v6, v3, v2, mesh);
    addTriFaceToMesh(v6, v4, v3, mesh);
    addTriFaceToMesh(v6, v5, v4, mesh);
    addTriFaceToMesh(v6, v2, v5, mesh);

    buildConnections(mesh);
}

void makeSharpOctahedron(Mesh &mesh){
    //unordered_map<uint, Face*>::iterator faceIt;
    //unordered_map<unsigned long long, Halfedge*>::iterator edgeIt;
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;
    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
 
    v1 -> position = vec3(0, 0, 1.414);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.414);

    addTriFaceToMesh(v1, v2, v3, mesh);
    addTriFaceToMesh(v1, v3, v4, mesh);
    addTriFaceToMesh(v1, v4, v5, mesh);
    addTriFaceToMesh(v1, v5, v2, mesh);
    addTriFaceToMesh(v6, v3, v2, mesh);
    addTriFaceToMesh(v6, v4, v3, mesh);
    addTriFaceToMesh(v6, v5, v4, mesh);
    addTriFaceToMesh(v6, v2, v5, mesh);

    buildConnections(mesh);
    //Should define sharp edge here.
}

void makeNormalStrip(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);

    v1 -> position = vec3(1.5, 1.5, 0);
    v2 -> position = vec3(1.5, -1.5, 0);
    v3 -> position = vec3(-1.5, -1.5, 0);
    v4 -> position = vec3(-1.5, 1.5, 0);
    v5 -> position = vec3(0.5, 0.5, 0);
    v6 -> position = vec3(0.5, -0.5, 0);
    v7 -> position = vec3(-0.5, -0.5, 0);
    v8 -> position = vec3(-0.5, 0.5, 0);

    addQuadFaceToMesh(v1, v5, v6, v2, mesh);
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);

    buildConnections(mesh);
}

void makeMobius(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;
    Vertex * v9 = new Vertex;
    Vertex * vX = new Vertex;

    v1 -> position = vec3(1.5, 1.5, 0);
    v2 -> position = vec3(1.5, -1.5, 0);
    v3 -> position = vec3(-1.5, -1.5, 0);
    v4 -> position = vec3(-1.5, 1.5, 0);
    v5 -> position = vec3(0.5, 0.5, 0);
    v6 -> position = vec3(0.5, -0.5, 0);
    v7 -> position = vec3(-0.5, -0.5, 0);
    v8 -> position = vec3(-0.5, 0.5, 0);
    v9 -> position = vec3(1, 0, 0.5);
    vX -> position = vec3(1, 0, -0.5);

    v1 -> ID = 0;
    v2 -> ID = 1;
    v3 -> ID = 2;
    v4 -> ID = 3;
    v5 -> ID = 4;
    v6 -> ID = 5;
    v7 -> ID = 6;
    v8 -> ID = 7;
    v9 -> ID = 8;
    vX -> ID = 9;

    //push on all new verts
    addVertexToMesh(v1, mesh);
    addVertexToMesh(v2, mesh);
    addVertexToMesh(v3, mesh);
    addVertexToMesh(v4, mesh);
    addVertexToMesh(v5, mesh);
    addVertexToMesh(v6, mesh);
    addVertexToMesh(v7, mesh);
    addVertexToMesh(v8, mesh);
    addVertexToMesh(v9, mesh);
    addVertexToMesh(vX, mesh);

    addQuadFaceToMesh(v1, v5, vX, v9, mesh);
    addQuadFaceToMesh(vX, v9, v6, v2, mesh);
    addQuadFaceToMesh(v3, v2, v6, v7, mesh);
    addQuadFaceToMesh(v4, v3, v7, v8, mesh);
    addQuadFaceToMesh(v1, v4, v8, v5, mesh);

    buildConnections(mesh);
}

void makeCircleSweep(Mesh &mesh) {
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    int loop_test = 0;
    int l_slices = 36;
    float total_arc = 360.0;
    float step = total_arc / l_slices;
    vector<vector<Vertex*> > vertices;

    for(float i = 0.0; i <= total_arc; i += step) {
        vector<Vertex*> crossSection;
        float angle = i / 180 * PI;
        vec3 zaxis = vec3(0, 0, 1);
        vec3 xaxis = vec3(1, 0, 0);
        vec3 v1 = vec3(0, 2, 1);
        vec3 v2 = vec3(0, 1, 2);
        vec3 v3 = vec3(0, -1, -2);
        vec3 v4 = vec3(0, -2, -1);
        vec3 trans = vec3(0 , 3, 0);

        // I will twist m * 180 of this sweep.
        int m = 1;

        v1 = rotate(v1, m*angle/2, xaxis);
        v2 = rotate(v2, m*angle/2, xaxis);
        v3 = rotate(v3, m*angle/2, xaxis);
        v4 = rotate(v4, m*angle/2, xaxis);
        
        v1 += trans;
        v2 += trans;
        v3 += trans;
        v4 += trans;

        v1 = rotate(v1, angle, zaxis);
        v2 = rotate(v2, angle, zaxis);
        v3 = rotate(v3, angle, zaxis);
        v4 = rotate(v4, angle, zaxis);

        if(i == total_arc) {
            if(distance(v1, vertices[0][0] -> position) < VERYSMALLVALUE
             && distance(v4, vertices[0][3] -> position) < VERYSMALLVALUE) {
                loop_test = 1;
                //cout<<"This is a normal loop."<<endl;
                break;
            } else if(distance(v1, vertices[0][3] -> position) < VERYSMALLVALUE
             && distance(v4, vertices[0][0] -> position) < VERYSMALLVALUE) {
                //cout<<"This is a moibus loop."<<endl;
                loop_test = 2;
                break;
            }
        }

        Vertex * P1 = new Vertex;
        Vertex * P2 = new Vertex;
        Vertex * P3 = new Vertex;
        Vertex * P4 = new Vertex;

        P1 -> position = v1;
        P2 -> position = v2;
        P3 -> position = v3;
        P4 -> position = v4;
        P1 -> ID = vertTable.size();
        addVertexToMesh(P1, mesh);
        P2 -> ID = vertTable.size();
        addVertexToMesh(P2, mesh);
        P3 -> ID = vertTable.size();
        addVertexToMesh(P3, mesh);
        P4 -> ID = vertTable.size();
        addVertexToMesh(P4, mesh);
        crossSection.push_back(P1);
        crossSection.push_back(P2);
        crossSection.push_back(P3);
        crossSection.push_back(P4);
        vertices.push_back(crossSection);
    }
    for(size_t j = 0; j < vertices.size() - 1; j += 1) {
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][k];
            Vertex * v2 = vertices[j + 1][k];
            Vertex * v3 = vertices[j + 1][k + 1];
            Vertex * v4 = vertices[j][k + 1];
            addQuadFaceToMesh(v1, v2, v3, v4, mesh);
        }
    }
    if(loop_test == 1) { //If it is a loop //allignment test
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][k];
            Vertex * v2 = vertices[0][k];
            Vertex * v3 = vertices[0][k + 1];
            Vertex * v4 = vertices[j][k + 1];
            addQuadFaceToMesh(v1, v2, v3, v4, mesh);
        }        
    }
    if(loop_test == 2) { //If it is a mobius loop
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][3 - k - 1];
            Vertex * v2 = vertices[j][3 - k];
            Vertex * v3 = vertices[0][k];
            Vertex * v4 = vertices[0][k + 1];
            addQuadFaceToMesh(v1, v2, v3, v4, mesh);
        }        
    }
    buildConnections(mesh);
}


void makeWithSIF(Mesh &mesh, string inputSIF){
    unordered_map<uint, Face*>::iterator faceIt;
    flushMesh(mesh);
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

    ifstream file(inputSIF);
    if (!file.good()) {
        cout<<"THE PATH OF SIF FILE IS NOT VAILD.";
        exit(1); // exit if file not found
    }
    string nextLine;
    regex anyR(".*(.*).*");
    regex vRegex(".*\(v .*$\).*");
    regex tRegex(".*\(t .*\).*");
    regex lRegex(".*\(loop .*\).*");
    regex shRegex(".*\\(shell.*\).*");
    regex verticesRegex(".*\\(vertices .*\).*");
    int vBeforeMergeCounter = 0;
    int vAfterMergeCounter = 0;
    int IDplusBecauseOfShells = 0;
    vector<vector<int> > boundaries;
    unordered_map<int, int> mapBeforeMergeToAfter;
    int shellNum = 0;
    vector<int> numberOfVerticesInShells;
    while(getline(file, nextLine)){
        nextLine.pop_back();
        if(regex_match(nextLine, vRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            float x = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float y = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float z = stof(temp);
            Vertex * newVert = new Vertex;
            newVert -> position = vec3(x, y, z) * 10.0f; // Can be modifed here to zoom in.
            newVert -> ID = vAfterMergeCounter;
            unordered_map<unsigned long, Vertex*>::iterator vIt;
            bool alreadyAdded = false;
            for (vIt = vertTable.begin(); vIt != vertTable.end(); vIt ++) {
                if(distance(newVert -> position, vIt -> second -> position) < 0.001 ){
                    //cout << "The distance between vertex "<<newVert -> ID<<" and vertex "<<vIt -> second -> ID<<" is: "<<endl;
                    //cout << newVert -> position - (*vIt) -> position<<endl;
                    alreadyAdded = true;
                    mapBeforeMergeToAfter[vBeforeMergeCounter] = vIt -> second -> ID;
                    break;
                }
            }
            //cout<<newVert -> ID<<"Vertex added"<<endl;
            //cout<<"I am mapping "<<vBeforeMergeCounter<<" to "<<vAfterMergeCounter<<endl;
            //mapBeforeMergeToAfter[vBeforeMergeCounter] = vAfterMergeCounter;

            if(!alreadyAdded) {
                newVert -> ID = vAfterMergeCounter;
                addVertexToMesh(newVert, mesh);
                mapBeforeMergeToAfter[vBeforeMergeCounter] = vAfterMergeCounter;
                vAfterMergeCounter += 1;
            }
            vBeforeMergeCounter += 1;
        } else if(regex_match(nextLine, tRegex)) {
            string temp;
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            //cout<<temp<<endl;
            temp = temp.substr(temp.find(" ") + 1);
            int a = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int b = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int c = stoi(temp);
            if(shellNum > 0) {
                vector<int>::iterator vertNumIt;
                IDplusBecauseOfShells = 0;
                for(vertNumIt = numberOfVerticesInShells.begin();
                 vertNumIt < numberOfVerticesInShells.end() - 1; vertNumIt ++) {
                    IDplusBecauseOfShells += *vertNumIt;
                }
            }
            a += IDplusBecauseOfShells;
            b += IDplusBecauseOfShells;
            c += IDplusBecauseOfShells;
            //cout<<"a: "<< a <<" b: "<<b<<" c: "<<c<<endl;
            a = mapBeforeMergeToAfter[a];
            b = mapBeforeMergeToAfter[b];
            c = mapBeforeMergeToAfter[c];
            Vertex * va = vertTable[a];
            Vertex * vb = vertTable[b];
            Vertex * vc = vertTable[c];
            //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            addTriFaceToMesh(va, vb, vc, mesh);
        } else if(regex_match(nextLine, lRegex)){
            vector<int> oneBoundary;
            string temp;
            int nextVert;
            if(shellNum > 1) {
                vector<int>::iterator vertNumIt;
                IDplusBecauseOfShells = 0;
                for(vertNumIt = numberOfVerticesInShells.begin();
                 vertNumIt < numberOfVerticesInShells.end(); vertNumIt ++) {
                    IDplusBecauseOfShells += *vertNumIt;
                }
            }
            temp = nextLine.substr(nextLine.find("\("),
             nextLine.find("\)") - nextLine.find("\("));
            //cout<<temp<<endl;
            temp = temp.substr(temp.find(" ") + 1);
            while(temp.find(" ") != string::npos){
                nextVert = stoi(temp.substr(0, temp.find(" ")));
                oneBoundary.push_back(nextVert + IDplusBecauseOfShells);
                temp = temp.substr(temp.find(" ") + 1);
            }
            nextVert = stoi(temp);
            oneBoundary.push_back(nextVert + IDplusBecauseOfShells);
            boundaries.push_back(oneBoundary);
            //cout<<oneBoundary.size()<<endl;          
        } else if(regex_match(nextLine, shRegex)) {
            shellNum += 1;
        } else if(regex_match(nextLine, verticesRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("),
             nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            int numberOfVerticesInThisShell = stoi(temp);
            numberOfVerticesInShells.push_back(numberOfVerticesInThisShell);
        }
    }
    buildConnections(mesh);
}


void makeWithQuadSIF(Mesh &mesh, string inputSIF){
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;
    unordered_map<uint, Face*>::iterator faceIt;
    //Flush the old mesh
    flushMesh(mesh);
    ifstream file(inputSIF);
    if (!file.good()) {
        exit(1); // exit if file not found
    }
    string nextLine;
    regex anyR(".*(.*).*");
    regex vRegex(".*\(v .*$\).*");
    regex tRegex(".*\(t .*\).*");
    regex lRegex(".*\(loop .*\).*");
    regex shRegex(".*\\(shell.*\).*");
    regex verticesRegex(".*\\(vertices .*\).*");
    int vCounter = 0;
    int IDplusBecauseOfShells = 0;
    int shellNum = 0;
    vector<int> numberOfVerticesInShells;
    int tCounter = 0;
    Vertex * va;
    Vertex * vb;
    Vertex * vc;
    Vertex * vd;
    while(getline(file, nextLine)){
        nextLine.pop_back();
        if(regex_match(nextLine, vRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("),
             nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            float x = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float y = stof(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            float z = stof(temp);
            Vertex * newVert = new Vertex;
            newVert -> position = vec3(x, y, z) * 16.0f; // Can be modifed here to zoom in.
            newVert -> ID = vCounter;
            addVertexToMesh(newVert, mesh);
            vCounter += 1;
        } else if(regex_match(nextLine, tRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("),
             nextLine.find("\)") - nextLine.find("\("));
            //cout<<temp<<endl;
            temp = temp.substr(temp.find(" ") + 1);
            int a = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int b = stoi(temp.substr(0, temp.find(" ")));
            temp = temp.substr(temp.find(" ") + 1);
            int c = stoi(temp);
            if(shellNum > 0) {
                vector<int>::iterator vertNumIt;
                IDplusBecauseOfShells = 0;
                for(vertNumIt = numberOfVerticesInShells.begin();
                 vertNumIt < numberOfVerticesInShells.end() - 1; vertNumIt ++) {
                    IDplusBecauseOfShells += *vertNumIt;
                }
            }
            a += IDplusBecauseOfShells;
            b += IDplusBecauseOfShells;
            c += IDplusBecauseOfShells;
            if(tCounter % 2 == 0) {
                va = vertTable[a];
                vb = vertTable[b];
                vc = vertTable[c];
            } else {
                vd = vertTable[c];
                addQuadFaceToMesh(va, vb, vc, vd, mesh);
            }
            tCounter += 1;
        } else if(regex_match(nextLine, shRegex)) {
            shellNum += 1;
        } else if(regex_match(nextLine, verticesRegex)){
            string temp;
            temp = nextLine.substr(nextLine.find("\("),
             nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            int numberOfVerticesInThisShell = stoi(temp);
            numberOfVerticesInShells.push_back(numberOfVerticesInThisShell);
        }
    }
    mergeSelfBoundary(mesh);
    buildConnections(mesh);
    //cout<<"Finish boundary!"<<endl;
}
#endif // __MAKEMESH_H__
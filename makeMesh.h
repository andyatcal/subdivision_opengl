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
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
    buildConnections(mesh);

}

void makePyramid(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);

    vector<Vertex*> bottomFace;

    bottomFace.push_back(v3);
    bottomFace.push_back(v2);
    bottomFace.push_back(v5);
    bottomFace.push_back(v4);
    //bottomFace
    makeRectFace(v3, v2, v5, v4, faceTable, edgeTable);
    //makeRectFace(v3, v2, v5, v4, faceTable, edgeTable);
    //topfrontFace
    makeTriFace(v1, v2, v3, faceTable, edgeTable);
    //topbackFace
    makeTriFace(v1, v3, v4, faceTable, edgeTable);
    //topleftFace
    makeTriFace(v1, v4, v5, faceTable, edgeTable);
    //toprightFace
    makeTriFace(v1, v5, v2, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);
 
    //topFace
    makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceTable, edgeTable);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceTable, edgeTable);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeOpenCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);

    //without the topFace
    //makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceTable, edgeTable);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceTable, edgeTable);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);

    //Boundaries
    buildConnections(mesh);
}

void makeRing(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);

    //leftFace
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceTable, edgeTable);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeSharpCube(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);

    //without the topFace
    makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceTable, edgeTable);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceTable, edgeTable);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);
    
    buildConnections(mesh);

    unordered_map<unsigned long long, Halfedge*>::iterator eIt;
    for(eIt = edgeTable.begin(); eIt != edgeTable.end(); eIt++) {
        if (eIt -> second -> start == v1 || eIt -> second -> end == v1) {
            eIt -> second -> isSharp = true;
        }
        /* 
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
        */
    }
}

void makeOctahedron(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
 
    v1 -> position = vec3(0, 0, 1.414);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.414);

    makeTriFace(v1, v2, v3, faceTable, edgeTable);
    makeTriFace(v1, v3, v4, faceTable, edgeTable);
    makeTriFace(v1, v4, v5, faceTable, edgeTable);
    makeTriFace(v1, v5, v2, faceTable, edgeTable);
    makeTriFace(v6, v3, v2, faceTable, edgeTable);
    makeTriFace(v6, v4, v3, faceTable, edgeTable);
    makeTriFace(v6, v5, v4, faceTable, edgeTable);
    makeTriFace(v6, v2, v5, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeSharpOctahedron(Mesh &mesh){
    unordered_map<uint, Face*>::iterator faceIt;
    unordered_map<unsigned long long, Halfedge*>::iterator edgeIt;
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
 
    v1 -> position = vec3(0, 0, 1.414);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.414);

    makeTriFace(v1, v2, v3, faceTable, edgeTable);
    makeTriFace(v1, v3, v4, faceTable, edgeTable);
    makeTriFace(v1, v4, v5, faceTable, edgeTable);
    makeTriFace(v1, v5, v2, faceTable, edgeTable);
    makeTriFace(v6, v3, v2, faceTable, edgeTable);
    makeTriFace(v6, v4, v3, faceTable, edgeTable);
    makeTriFace(v6, v5, v4, faceTable, edgeTable);
    makeTriFace(v6, v2, v5, faceTable, edgeTable);

    buildConnections(mesh);
    //Should define sharp edge here.
}

void makeNormalStrip(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);

    v1 -> position = vec3(1.5, 1.5, 0);
    v2 -> position = vec3(1.5, -1.5, 0);
    v3 -> position = vec3(-1.5, -1.5, 0);
    v4 -> position = vec3(-1.5, 1.5, 0);
    v5 -> position = vec3(0.5, 0.5, 0);
    v6 -> position = vec3(0.5, -0.5, 0);
    v7 -> position = vec3(-0.5, -0.5, 0);
    v8 -> position = vec3(-0.5, 0.5, 0);

    makeRectFace(v1, v5, v6, v2, faceTable, edgeTable);
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeMobius(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
    v1 -> addToHashTable(vertTable);
    v2 -> addToHashTable(vertTable);
    v3 -> addToHashTable(vertTable);
    v4 -> addToHashTable(vertTable);
    v5 -> addToHashTable(vertTable);
    v6 -> addToHashTable(vertTable);
    v7 -> addToHashTable(vertTable);
    v8 -> addToHashTable(vertTable);
    v9 -> addToHashTable(vertTable);
    vX -> addToHashTable(vertTable);

    makeRectFace(v1, v5, vX, v9, faceTable, edgeTable);
    makeRectFace(vX, v9, v6, v2, faceTable, edgeTable);
    makeRectFace(v3, v2, v6, v7, faceTable, edgeTable);
    makeRectFace(v4, v3, v7, v8, faceTable, edgeTable);
    makeRectFace(v1, v4, v8, v5, faceTable, edgeTable);

    buildConnections(mesh);
}

void makeHild(Mesh &mesh){
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;
    //make new mesh
    Vertex * vertices[22];
    for(int i = 0; i < 22; i += 1) {
        vertices[i] = new Vertex;
        vertices[i] -> ID = i - 1;
        vertices[i] -> addToHashTable(vertTable);
    }

    //add positions to vertices
    vertices[0] -> position = vec3(0.63553, 0.81293, -0.15891);
    vertices[1] -> position = vec3(-0.42251, 0.86460, 0.40491);
    vertices[2] -> position = vec3(0.08171, 0.14334, -0.07264);
    vertices[3] -> position = vec3(-0.78509, -0.66737, -0.13510);
    vertices[4] -> position = vec3(-0.70442, 0.76199, -0.11471);
    vertices[5] -> position = vec3(0.35362, 0.71032, -0.67852);
    vertices[6] -> position = vec3(0.67504, 0.14533, -1.08775);
    vertices[7] -> position = vec3(0.23617, -0.91369, -0.64053);
    vertices[8] -> position = vec3(-0.20207, -0.89228, -0.40699);
    vertices[9] -> position = vec3(-0.34683, -0.81704, 0.06254);
    vertices[10] -> position = vec3(-0.22128, -0.46216, 0.90413);
    vertices[11] -> position = vec3(-0.11330, -0.73205, 0.49299);
    vertices[12] -> position = vec3(0.36174, -0.68707, 0.63224);
    vertices[13] -> position = vec3(0.79998, -0.70847, 0.39870);
    vertices[14] -> position = vec3(0.94474, -0.78372, -0.07083);
    vertices[15] -> position = vec3(0.71121, -0.86871, -0.50128);
    vertices[16] -> position = vec3(1.23886, 0.35054, -0.04852);
    vertices[17] -> position = vec3(0.25805, 0.13473, -0.16661);
    vertices[18] -> position = vec3(0.21769, 0.05164, -0.12830);
    vertices[19] -> position = vec3(0.12952, 0.05595, -0.08132);
    vertices[20] -> position = vec3(0.21024, 0.22213, -0.15792);
    vertices[21] -> position = vec3(0.12207, 0.22643, -0.11094);

    //make faces
    makeTriFace(vertices[0], vertices[20], vertices[17], faceTable, edgeTable);
    makeTriFace(vertices[0], vertices[17], vertices[16], faceTable, edgeTable);
    makeTriFace(vertices[1], vertices[21], vertices[20], faceTable, edgeTable);
    makeTriFace(vertices[1], vertices[20], vertices[0], faceTable, edgeTable);
    makeTriFace(vertices[2], vertices[21], vertices[1], faceTable, edgeTable);
    makeTriFace(vertices[2], vertices[1], vertices[10], faceTable, edgeTable);
    makeTriFace(vertices[3], vertices[4], vertices[21], faceTable, edgeTable);
    makeTriFace(vertices[3], vertices[21], vertices[2], faceTable, edgeTable);
    makeTriFace(vertices[4], vertices[5], vertices[20], faceTable, edgeTable);
    makeTriFace(vertices[4], vertices[20], vertices[21], faceTable, edgeTable);
    makeTriFace(vertices[6], vertices[17], vertices[20], faceTable, edgeTable);
    makeTriFace(vertices[6], vertices[20], vertices[5], faceTable, edgeTable);
    makeTriFace(vertices[7], vertices[17], vertices[6], faceTable, edgeTable);
    makeTriFace(vertices[7], vertices[6], vertices[15], faceTable, edgeTable);
    makeTriFace(vertices[8], vertices[18], vertices[17], faceTable, edgeTable);
    makeTriFace(vertices[8], vertices[17], vertices[7], faceTable, edgeTable);
    makeTriFace(vertices[9], vertices[19], vertices[18], faceTable, edgeTable);
    makeTriFace(vertices[9], vertices[18], vertices[8], faceTable, edgeTable);
    makeTriFace(vertices[9], vertices[3], vertices[2], faceTable, edgeTable);
    makeTriFace(vertices[9], vertices[2], vertices[19], faceTable, edgeTable);
    makeTriFace(vertices[10], vertices[3], vertices[9], faceTable, edgeTable);
    makeTriFace(vertices[10], vertices[9], vertices[11], faceTable, edgeTable);
    makeTriFace(vertices[11], vertices[19], vertices[2], faceTable, edgeTable);
    makeTriFace(vertices[11], vertices[2], vertices[10], faceTable, edgeTable);
    makeTriFace(vertices[12], vertices[18], vertices[19], faceTable, edgeTable);
    makeTriFace(vertices[12], vertices[19], vertices[11], faceTable, edgeTable);
    makeTriFace(vertices[13], vertices[17], vertices[18], faceTable, edgeTable);
    makeTriFace(vertices[13], vertices[18], vertices[12], faceTable, edgeTable);
    makeTriFace(vertices[14], vertices[16], vertices[17], faceTable, edgeTable);
    makeTriFace(vertices[14], vertices[17], vertices[13], faceTable, edgeTable);
    makeTriFace(vertices[15], vertices[6], vertices[16], faceTable, edgeTable);
    makeTriFace(vertices[15], vertices[16], vertices[14], faceTable, edgeTable);

    buildConnections(mesh);
}

void makeCircleSweep(Mesh &mesh) {
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
        P1 -> addToHashTable(vertTable);
        P2 -> ID = vertTable.size();
        P2 -> addToHashTable(vertTable);
        P3 -> ID = vertTable.size();
        P3 -> addToHashTable(vertTable);
        P4 -> ID = vertTable.size();
        P4 -> addToHashTable(vertTable);

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
            makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
        }
    }
    if(loop_test == 1) { //If it is a loop //allignment test
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][k];
            Vertex * v2 = vertices[0][k];
            Vertex * v3 = vertices[0][k + 1];
            Vertex * v4 = vertices[j][k + 1];
            makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
        }        
    }
    if(loop_test == 2) { //If it is a mobius loop
        size_t j = vertices.size() - 1;
        for(size_t k = 0; k < 3; k += 1){
            Vertex * v1 = vertices[j][3 - k - 1];
            Vertex * v2 = vertices[j][3 - k];
            Vertex * v3 = vertices[0][k];
            Vertex * v4 = vertices[0][k + 1];
            makeRectFace(v1, v2, v3, v4, faceTable, edgeTable);
        }        
    }
    buildConnections(mesh);
}

void makeWithSIF(Mesh &mesh, string inputSIF){
    unordered_map<uint, Face*>::iterator faceIt;
    unordered_map<unsigned long long, Halfedge*>::iterator edgeIt;
    flushMesh(mesh);
    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
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
                newVert -> addToHashTable(vertTable);
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
                for(vertNumIt = numberOfVerticesInShells.begin(); vertNumIt < numberOfVerticesInShells.end() - 1; vertNumIt ++) {
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
            /*
            if (it != mergeVertex.end()){
                a = it -> second;
            }
            it = mergeVertex.find(b);
            if (it != mergeVertex.end()) {
                b = it -> second;
            }
            it = mergeVertex.find(c);
            if (it != mergeVertex.end()){
                c = it -> second;
            }
            */
            Vertex * va = vertTable[a];
            Vertex * vb = vertTable[b];
            Vertex * vc = vertTable[c];
            //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            makeTriFace(va, vb, vc, faceTable, edgeTable);
        } else if(regex_match(nextLine, lRegex)){
            vector<int> oneBoundary;
            string temp;
            int nextVert;
            if(shellNum > 1) {
                vector<int>::iterator vertNumIt;
                IDplusBecauseOfShells = 0;
                for(vertNumIt = numberOfVerticesInShells.begin(); vertNumIt < numberOfVerticesInShells.end(); vertNumIt ++) {
                    IDplusBecauseOfShells += *vertNumIt;
                }
            }
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
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
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            int numberOfVerticesInThisShell = stoi(temp);
            numberOfVerticesInShells.push_back(numberOfVerticesInThisShell);
        }
    }
    buildConnections(mesh);
}

void makeWithQuadSIF(Mesh &mesh, string inputSIF){
    unordered_map<uint, Face*>::iterator faceIt;
    unordered_map<unsigned long long, Halfedge*>::iterator edgeIt;
    //Flush the old mesh
    flushMesh(mesh);

    unordered_map<uint, Face*> &faceTable = mesh.faceTable;
    unordered_map<unsigned long long, Halfedge*> &edgeTable = mesh.edgeTable;
    unordered_map<unsigned long, Vertex*> &vertTable = mesh.vertTable;

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
    int vBeforeMergeCounter = 0;
    int vAfterMergeCounter = 0;
    int IDplusBecauseOfShells = 0;
    vector<vector<int> > boundaries;
    unordered_map<int, int> mapBeforeMergeToAfter;
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
                newVert -> addToHashTable(vertTable);
                mapBeforeMergeToAfter[vBeforeMergeCounter] = vAfterMergeCounter;
                vAfterMergeCounter += 1;
            }
            vBeforeMergeCounter += 1;
        } else if(regex_match(nextLine, tRegex)){
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
                for(vertNumIt = numberOfVerticesInShells.begin(); vertNumIt < numberOfVerticesInShells.end() - 1; vertNumIt ++) {
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
            //cout<<"tCounter: "<<tCounter<<endl;
            if(tCounter % 2 == 0) {
                va = vertTable[a];
                vb = vertTable[b];
                vc = vertTable[c];
                //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            } else {
                vd = vertTable[c];
                //cout<<vd -> ID<<endl;
                makeRectFace(va, vb, vc, vd, faceTable, edgeTable);
            }
            tCounter += 1;
        } else if(regex_match(nextLine, lRegex)){
            vector<int> oneBoundary;
            string temp;
            int nextVert;
            if(shellNum > 1) {
                vector<int>::iterator vertNumIt;
                IDplusBecauseOfShells = 0;
                for(vertNumIt = numberOfVerticesInShells.begin(); vertNumIt < numberOfVerticesInShells.end(); vertNumIt ++) {
                    IDplusBecauseOfShells += *vertNumIt;
                }
            }
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
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
            temp = nextLine.substr(nextLine.find("\("), nextLine.find("\)") - nextLine.find("\("));
            temp = temp.substr(temp.find(" ") + 1);
            int numberOfVerticesInThisShell = stoi(temp);
            numberOfVerticesInShells.push_back(numberOfVerticesInThisShell);
        }
    }
    buildConnections(mesh);
}

#endif // __MAKEMESH_H__
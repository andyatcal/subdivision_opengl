// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __MAKEMESH_H__
#define __MAKEMESH_H__

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

void makePyramid(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
 
    v1 -> position = vec3(0, 0, 1);
    v2 -> position = vec3(1, 1, -1);
    v3 -> position = vec3(1, -1, -1);
    v4 -> position = vec3(-1, -1, -1);
    v5 -> position = vec3(-1, 1, -1);

    vector<Vertex*> bottomFace;
    bottomFace.push_back(v3);
    bottomFace.push_back(v2);
    bottomFace.push_back(v5);
    bottomFace.push_back(v4);
    //bottomFace
    makePolygonFace(bottomFace, faceVect, edgeVect);
    //makeRectFace(v3, v2, v5, v4, faceVect, edgeVect);
    //topfrontFace
    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    //topbackFace
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    //topleftFace
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    //toprightFace
    makeTriFace(v1, v5, v2, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeCube(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);
    v1 -> ID = 1;
    v2 -> ID = 2;
    v3 -> ID = 3;
    v4 -> ID = 4;
    v5 -> ID = 5;
    v6 -> ID = 6;
    v7 -> ID = 7;
    v8 -> ID = 8;
    //topFace
    makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeOpenCube(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //without the topFace
    //makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    //Boundaries
    buildConnections(mesh);
}

void makeRing(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeSharpCube(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = vec3(1, 1, 1);
    v2 -> position = vec3(1, -1, 1);
    v3 -> position = vec3(-1, -1, 1);
    v4 -> position = vec3(-1, 1, 1);
    v5 -> position = vec3(1, 1, -1);
    v6 -> position = vec3(1, -1, -1);
    v7 -> position = vec3(-1, -1, -1);
    v8 -> position = vec3(-1, 1, -1);

    //without the topFace
    makeRectFace(v1, v2, v3, v4, faceVect, edgeVect);
    //bottomFace
    makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    
    buildConnections(mesh);
}

void makeOctahedron(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
 
    v1 -> position = vec3(0, 0, 1.414);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.414);


    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    makeTriFace(v1, v5, v2, faceVect, edgeVect);
    makeTriFace(v6, v3, v2, faceVect, edgeVect);
    makeTriFace(v6, v4, v3, faceVect, edgeVect);
    makeTriFace(v6, v5, v4, faceVect, edgeVect);
    makeTriFace(v6, v2, v5, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeSharpOctahedron(Mesh &mesh){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;
    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
 
    v1 -> position = vec3(0, 0, 1.4);
    v2 -> position = vec3(1, 1, 0);
    v3 -> position = vec3(1, -1, 0);
    v4 -> position = vec3(-1, -1, 0);
    v5 -> position = vec3(-1, 1, 0);
    v6 -> position = vec3(0, 0, -1.4);


    makeTriFace(v1, v2, v3, faceVect, edgeVect);
    makeTriFace(v1, v3, v4, faceVect, edgeVect);
    makeTriFace(v1, v4, v5, faceVect, edgeVect);
    makeTriFace(v1, v5, v2, faceVect, edgeVect);
    makeTriFace(v6, v3, v2, faceVect, edgeVect);
    makeTriFace(v6, v4, v3, faceVect, edgeVect);
    makeTriFace(v6, v5, v4, faceVect, edgeVect);
    makeTriFace(v6, v2, v5, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeNormalStrip(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

    //make new mesh
    Vertex * v1 = new Vertex;
    Vertex * v2 = new Vertex;
    Vertex * v3 = new Vertex;
    Vertex * v4 = new Vertex;
    Vertex * v5 = new Vertex;
    Vertex * v6 = new Vertex;
    Vertex * v7 = new Vertex;
    Vertex * v8 = new Vertex;

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
 
    v1 -> position = vec3(1.5, 1.5, 0);
    v2 -> position = vec3(1.5, -1.5, 0);
    v3 -> position = vec3(-1.5, -1.5, 0);
    v4 -> position = vec3(-1.5, 1.5, 0);
    v5 -> position = vec3(0.5, 0.5, 0);
    v6 -> position = vec3(0.5, -0.5, 0);
    v7 -> position = vec3(-0.5, -0.5, 0);
    v8 -> position = vec3(-0.5, 0.5, 0);

    makeRectFace(v1, v5, v6, v2, faceVect, edgeVect);
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeMobius(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

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

    //push on all new verts
    vertVect.push_back(v1);
    vertVect.push_back(v2);
    vertVect.push_back(v3);
    vertVect.push_back(v4);
    vertVect.push_back(v5);
    vertVect.push_back(v6);
    vertVect.push_back(v7);
    vertVect.push_back(v8);
    vertVect.push_back(v9);
    vertVect.push_back(vX);

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

    v1 -> ID = 1;
    v2 -> ID = 2;
    v3 -> ID = 3;
    v4 -> ID = 4;
    v5 -> ID = 5;
    v6 -> ID = 6;
    v7 -> ID = 7;
    v8 -> ID = 8;
    v9 -> ID = 9;
    vX -> ID = 10;

    makeRectFace(v1, v5, vX, v9, faceVect, edgeVect);
    makeRectFace(vX, v9, v6, v2, faceVect, edgeVect);
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);

    buildConnections(mesh);
}

void makeHild(Mesh &mesh){
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;
    //make new mesh
    Vertex * vertices[22];
    for(int i = 0; i < 22; i += 1) {
        vertices[i] = new Vertex;
        vertices[i] -> ID = i;
        vertVect.push_back(vertices[i]);
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
    makeTriFace(vertices[0], vertices[20], vertices[17], faceVect, edgeVect);
    makeTriFace(vertices[0], vertices[17], vertices[16], faceVect, edgeVect);
    makeTriFace(vertices[1], vertices[21], vertices[20], faceVect, edgeVect);
    makeTriFace(vertices[1], vertices[20], vertices[0], faceVect, edgeVect);
    makeTriFace(vertices[2], vertices[21], vertices[1], faceVect, edgeVect);
    makeTriFace(vertices[2], vertices[1], vertices[10], faceVect, edgeVect);
    makeTriFace(vertices[3], vertices[4], vertices[21], faceVect, edgeVect);
    makeTriFace(vertices[3], vertices[21], vertices[2], faceVect, edgeVect);
    makeTriFace(vertices[4], vertices[5], vertices[20], faceVect, edgeVect);
    makeTriFace(vertices[4], vertices[20], vertices[21], faceVect, edgeVect);
    makeTriFace(vertices[6], vertices[17], vertices[20], faceVect, edgeVect);
    makeTriFace(vertices[6], vertices[20], vertices[5], faceVect, edgeVect);
    makeTriFace(vertices[7], vertices[17], vertices[6], faceVect, edgeVect);
    makeTriFace(vertices[7], vertices[6], vertices[15], faceVect, edgeVect);
    makeTriFace(vertices[8], vertices[18], vertices[17], faceVect, edgeVect);
    makeTriFace(vertices[8], vertices[17], vertices[7], faceVect, edgeVect);
    makeTriFace(vertices[9], vertices[19], vertices[18], faceVect, edgeVect);
    makeTriFace(vertices[9], vertices[18], vertices[8], faceVect, edgeVect);
    makeTriFace(vertices[9], vertices[3], vertices[2], faceVect, edgeVect);
    makeTriFace(vertices[9], vertices[2], vertices[19], faceVect, edgeVect);
    makeTriFace(vertices[10], vertices[3], vertices[9], faceVect, edgeVect);
    makeTriFace(vertices[10], vertices[9], vertices[11], faceVect, edgeVect);
    makeTriFace(vertices[11], vertices[19], vertices[2], faceVect, edgeVect);
    makeTriFace(vertices[11], vertices[2], vertices[10], faceVect, edgeVect);
    makeTriFace(vertices[12], vertices[18], vertices[19], faceVect, edgeVect);
    makeTriFace(vertices[12], vertices[19], vertices[11], faceVect, edgeVect);
    makeTriFace(vertices[13], vertices[17], vertices[18], faceVect, edgeVect);
    makeTriFace(vertices[13], vertices[18], vertices[12], faceVect, edgeVect);
    makeTriFace(vertices[14], vertices[16], vertices[17], faceVect, edgeVect);
    makeTriFace(vertices[14], vertices[17], vertices[13], faceVect, edgeVect);
    makeTriFace(vertices[15], vertices[6], vertices[16], faceVect, edgeVect);
    makeTriFace(vertices[15], vertices[16], vertices[14], faceVect, edgeVect);

    buildConnections(mesh);
}

void makeWithSIF(Mesh &mesh, string inputSIF){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    flushMesh(mesh);
    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

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
    int vCounter = 0;
    int IDplusBecauseOfShells = 0;
    vector<vector<int> > boundaries;
    map<int, int> mergeVertex;
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
            newVert -> position = vec3(x, y, z) * 20.0f; // Can be modifed here to zoom in.
            vector<Vertex*>::iterator vIt;
            newVert -> ID = vCounter;
            for (vIt = vertVect.begin(); vIt < vertVect.end(); vIt ++) {
                if(distance(newVert -> position, (*vIt) -> position) < VERYSMALLVALUE ){
                    //cout << "The distance between vertex "<<newVert -> ID<<" and vertex "<<(*vIt) -> ID<<" is: "<<endl;
                    //cout << newVert -> position - (*vIt) -> position<<endl;
                    mergeVertex[newVert -> ID] = (*vIt) -> ID;
                }
            }
            //cout<<newVert -> ID<<"Vertex added"<<endl;
            vertVect.push_back(newVert);
            vCounter += 1;
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
            auto it = mergeVertex.find(a);
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
            Vertex * va = vertVect[a];
            Vertex * vb = vertVect[b];
            Vertex * vc = vertVect[c];
            //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            makeTriFace(va, vb, vc, faceVect, edgeVect);
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
    //Siblings
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 + 1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) && ((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            } else if (((*edgeIt1) -> start == (*edgeIt2) -> start ) &&((*edgeIt1) -> end  == (*edgeIt2) -> end)) {

                (*edgeIt1)->mobiusSibling = *edgeIt2;
                (*edgeIt2)->mobiusSibling = *edgeIt1;

                (*edgeIt1) -> start -> onMobiusSibling = true;
                (*edgeIt1) -> end -> onMobiusSibling = true;
                (*edgeIt2) -> start -> onMobiusSibling = true;
                (*edgeIt2) -> end -> onMobiusSibling = true;

            }
        }
    }
    //Boundaries
    vector<vector<int> >::iterator boundIt;
    vector<vector<Vertex*> > boundaryVertices;
    for( boundIt = boundaries.begin(); boundIt < boundaries.end(); boundIt ++) {
        vector<Vertex*> oneBoundary;
        vector<int>::iterator oneBoundIt;
        for( oneBoundIt = (*boundIt).end() - 1; oneBoundIt >= (*boundIt).begin(); oneBoundIt--){
            auto it = mergeVertex.find(*oneBoundIt);
            if (it != mergeVertex.end()) {
                oneBoundary.push_back(vertVect[it -> second]);
                //cout<<"pushed: "<<it -> second<<endl;
            } else {
                oneBoundary.push_back(vertVect[*oneBoundIt]);
                //cout<<"pushed: "<<*oneBoundIt;
            }
        }
        boundaryVertices.push_back(oneBoundary);
    }
    makeBoundaries(boundaryVertices, edgeVect);
    vector<Vertex*> newVertVect;
    vector<Vertex*>::iterator vIt;
    for (vIt = vertVect.begin(); vIt < vertVect.end(); vIt ++) {
        auto it = mergeVertex.find((*vIt) -> ID);
        if(it == mergeVertex.end()){
            newVertVect.push_back((*vIt));
        }
    }
    vertVect = newVertVect;
}

void makeWithQuadSIF(Mesh &mesh, string inputSIF){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    //Flush the old mesh
    flushMesh(mesh);

    vector<Face*> &faceVect = mesh.faceVect;
    vector<Halfedge*> &edgeVect = mesh.edgeVect;
    vector<Vertex*> &vertVect = mesh.vertVect;

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
    vector<vector<int> > boundaries;
    map<int, int> mergeVertex;
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
            newVert -> position = vec3(x, y, z) * 16.0f; // Can be modifed here to zoom in.
            vector<Vertex*>::iterator vIt;
            newVert -> ID = vCounter;
            for (vIt = vertVect.begin(); vIt < vertVect.end(); vIt ++) {
                if(distance(newVert -> position, (*vIt) -> position) < VERYSMALLVALUE ){
                    //cout << "The distance between vertex "<<newVert -> ID<<" and vertex "<<(*vIt) -> ID<<" is: "<<endl;
                    //cout << newVert -> position - (*vIt) -> position<<endl;
                    mergeVertex[newVert -> ID] = (*vIt) -> ID;
                }
            }
            //cout<<newVert -> ID<<"Vertex added"<<endl;
            vertVect.push_back(newVert);
            vCounter += 1;
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
            auto it = mergeVertex.find(a);
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
            //cout<<"tCounter: "<<tCounter<<endl;
            if(tCounter % 2 == 0) {
                va = vertVect[a];
                vb = vertVect[b];
                vc = vertVect[c];
                //cout<<va -> ID<<" "<<vb -> ID<<" "<<vc -> ID<<endl;
            } else {
                vd = vertVect[c];
                //cout<<vd -> ID<<endl;
                makeRectFace(va, vb, vc, vd, faceVect, edgeVect);
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
    //Siblings
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 + 1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) && ((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            } else if (((*edgeIt1) -> start -> position == (*edgeIt2) -> start -> position) &&((*edgeIt1) -> end -> position == (*edgeIt2) -> end -> position)) {

                (*edgeIt1)->mobiusSibling = *edgeIt2;
                (*edgeIt2)->mobiusSibling = *edgeIt1;

                (*edgeIt1) -> start -> onMobiusSibling = true;
                (*edgeIt1) -> end -> onMobiusSibling = true;
                (*edgeIt2) -> start -> onMobiusSibling = true;
                (*edgeIt2) -> end -> onMobiusSibling = true;

            }
        }
    }
    //Boundaries
    vector<vector<int> >::iterator boundIt;
    vector<vector<Vertex*> > boundaryVertices;
    for( boundIt = boundaries.begin(); boundIt < boundaries.end(); boundIt ++) {
        vector<Vertex*> oneBoundary;
        vector<int>::iterator oneBoundIt;
        for( oneBoundIt = (*boundIt).end() - 1; oneBoundIt >= (*boundIt).begin(); oneBoundIt--){
            auto it = mergeVertex.find(*oneBoundIt);
            if (it != mergeVertex.end()) {
                oneBoundary.push_back(vertVect[it -> second]);
                //cout<<"pushed: "<<it -> second<<endl;
            } else {
                oneBoundary.push_back(vertVect[*oneBoundIt]);
                //cout<<"pushed: "<<*oneBoundIt;
            }
        }
        boundaryVertices.push_back(oneBoundary);
    }
    makeBoundaries(boundaryVertices, edgeVect);
    vector<Vertex*> newVertVect;
    vector<Vertex*>::iterator vIt;
    for (vIt = vertVect.begin(); vIt < vertVect.end(); vIt ++) {
        auto it = mergeVertex.find((*vIt) -> ID);
        if(it == mergeVertex.end()){
            newVertVect.push_back((*vIt));
        }
    }
    vertVect = newVertVect;
}
#endif // __MAKEMESH_H__
// Catmull-Clark Subdivision Project
// With sharp edge feature.
// With Mobius edge feature.
// Andy Wang, UC Berkeley July 1st 2015

#include <GLUT/GLUT.h>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>
#include "vertex.h"
#include "halfedge.h"
#include "face.h"
#include "mesh.h"
#include "subdivision.h"
#include "meshUtils.h"
#include "viewport.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209

using namespace std;
using namespace glm;

//************************************************************
//          Global Variables
//************************************************************

// The viewport to diaplay graphics in OpenGL
Viewport viewport;
// USAGE MESSAGE
string USAGE = "USAGE: ./cc SUBDIVSION_LEVEL SIFFILE(OPTIONAL)";
//Initial state of wireframe mode
bool wireframe = false;
//Initial state of smooth shading
bool smoothShading = false;
//Initial state of GL_Light
bool lightOn = false; 
//Initial Rotation Angle
float angle = 0.0; 
// Define a very small value
float VERYSMALLVALUE = 0.001;
// The mesh to subdivide and display.
Mesh glMesh;

//************************************************************
//          Let's build some Shapes!!
//************************************************************

void makePyramid(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeOpenCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;
    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeRing(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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
    //makeRectFace(v6, v5, v8, v7, faceVect, edgeVect);
    //leftFace
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    //rightFace
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);
    //frontFace
    makeRectFace(v2, v1, v5, v6, faceVect, edgeVect);
    //baceFace
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);

    //Boundaries

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

    e56 -> previousBoundary = e85;
    e56 -> nextBoundary = e67;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e56;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e56;
    e67 -> nextBoundary = e78;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeSharpCube(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    //Boundaries

    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    Halfedge * e12;
    Halfedge * e41;
    Halfedge * e34;
    Halfedge * e23;

    Halfedge * e65;
    Halfedge * e76;
    Halfedge * e87;
    Halfedge * e58;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v1) {
            e41 = *eIt;
        }
        if((*eIt) -> start == v1 && (*eIt) -> end == v2) {
            e12 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v3) {
            e23 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v4) {
            e34 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v5) {
            e65 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v6) {
            e76 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v7) {
            e87 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v8) {
            e58 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e12 -> isSharp = true;
    e41 -> isSharp = true;
    e34 -> isSharp = true;
    e23 -> isSharp = true;

    e65 -> isSharp = true;
    e76 -> isSharp = true;
    e87 -> isSharp = true;
    e58 -> isSharp = true;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeOctahedron(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeSharpOctahedron(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator eIt;
    Halfedge * e23;
    Halfedge * e34;
    Halfedge * e45;
    Halfedge * e52;

    Halfedge * e32;
    Halfedge * e43;
    Halfedge * e54;
    Halfedge * e25;

    Halfedge * e13;
    Halfedge * e36;
    Halfedge * e65;
    Halfedge * e51;

    Halfedge * e31;
    Halfedge * e63;
    Halfedge * e56;
    Halfedge * e15;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v2 && (*eIt) -> end == v3) {
            e23 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v4) {
            e34 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v5) {
            e45 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v2) {
            e52 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v4) {
            e54 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v5) {
            e25 = *eIt;
        }

        if((*eIt) -> start == v1 && (*eIt) -> end == v3) {
            e13 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v6) {
            e36 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v5) {
            e65 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v1) {
            e51 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v1) {
            e31 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v3) {
            e63 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v1 && (*eIt) -> end == v5) {
            e15 = *eIt;
        }
    }

    e23 -> isSharp = true;
    e34 -> isSharp = true;
    e45 -> isSharp = true;
    e52 -> isSharp = true;

    e32 -> isSharp = true;
    e43 -> isSharp = true;
    e54 -> isSharp = true;
    e25 -> isSharp = true;

    e13 -> isSharp = true;
    e36 -> isSharp = true;
    e65 -> isSharp = true;
    e51 -> isSharp = true;

    e31 -> isSharp = true;
    e63 -> isSharp = true;
    e56 -> isSharp = true;
    e15 -> isSharp = true;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeNormalStrip(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    //Boundaries
    vector<Halfedge*>::iterator eIt;
    Halfedge * e21;
    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    Halfedge * e56;
    Halfedge * e67;
    Halfedge * e78;
    Halfedge * e85;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == v1) {
            e21 = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == v6) {
            e56 = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
    }

    e21 -> isSharp = true;
    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;

    e56 -> isSharp = true;
    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;

    e21 -> previousBoundary = e32;
    e21 -> nextBoundary = e14;
    e14 -> previousBoundary = e21;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e21;

    e56 -> previousBoundary = e85;
    e56 -> nextBoundary = e67;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e56;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e56;
    e67 -> nextBoundary = e78;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
}

void makeMobius(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    v9 -> onMobiusSibling = true;
    vX -> onMobiusSibling = true;

    makeRectFace(v1, v5, vX, v9, faceVect, edgeVect);
    makeRectFace(vX, v9, v6, v2, faceVect, edgeVect);
    makeRectFace(v3, v2, v6, v7, faceVect, edgeVect);
    makeRectFace(v4, v3, v7, v8, faceVect, edgeVect);
    makeRectFace(v1, v4, v8, v5, faceVect, edgeVect);

    //Boundaries
    vector<Halfedge*>::iterator eIt;

    Halfedge * e14;
    Halfedge * e43;
    Halfedge * e32;

    //Special case at Mobius edge
    Halfedge * e2X;
    Halfedge * e5X;

    Halfedge * e85;
    Halfedge * e78;
    Halfedge * e67;

    //Special case at Mobius edge
    Halfedge * e96;
    Halfedge * e91;

    //Mobius edge
    Halfedge * eX9_1;
    Halfedge * eX9_2;

    bool oneMobiusAdded = false;

    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == v1 && (*eIt) -> end == v4) {
            e14 = *eIt;
        }
        if((*eIt) -> start == v2 && (*eIt) -> end == vX) {
            e2X = *eIt;
        }
        if((*eIt) -> start == v3 && (*eIt) -> end == v2) {
            e32 = *eIt;
        }
        if((*eIt) -> start == v4 && (*eIt) -> end == v3) {
            e43 = *eIt;
        }
        if((*eIt) -> start == v5 && (*eIt) -> end == vX) {
            e5X = *eIt;
        }
        if((*eIt) -> start == v6 && (*eIt) -> end == v7) {
            e67 = *eIt;
        }
        if((*eIt) -> start == v7 && (*eIt) -> end == v8) {
            e78 = *eIt;
        }
        if((*eIt) -> start == v8 && (*eIt) -> end == v5) {
            e85 = *eIt;
        }
        if((*eIt) -> start == v9 && (*eIt) -> end == v1) {
            e91 = *eIt;
        }
        if((*eIt) -> start == v9 && (*eIt) -> end == v6) {
            e96 = *eIt;
        }
        if((*eIt) -> start == vX && (*eIt) -> end == v9 && !oneMobiusAdded) {
            eX9_1 = *eIt;
            oneMobiusAdded = true;
        } else if((*eIt) -> start == vX && (*eIt) -> end == v9) {
            eX9_2 = *eIt;
        }
    }

    e14 -> isSharp = true;
    e43 -> isSharp = true;
    e32 -> isSharp = true;
    e2X -> isSharp = true;
    e5X -> isSharp = true;

    e67 -> isSharp = true;
    e78 -> isSharp = true;
    e85 -> isSharp = true;
    e96 -> isSharp = true;
    e91 -> isSharp = true;


    e2X -> previousBoundary = e32;
    e14 -> previousBoundary = e91;
    e14 -> nextBoundary = e43;
    e43 -> previousBoundary = e14;
    e43 -> nextBoundary = e32;
    e32 -> previousBoundary = e43;
    e32 -> nextBoundary = e2X;
    e5X -> previousBoundary = e85;
    e85 -> previousBoundary = e78;
    e85 -> nextBoundary = e5X;
    e78 -> previousBoundary = e67;
    e78 -> nextBoundary = e85;
    e67 -> previousBoundary = e96;
    e67 -> nextBoundary = e78;
    e96 -> nextBoundary = e67;

    e2X -> mobiusBoundary = e5X;
    e5X -> mobiusBoundary = e2X;
    e96 -> mobiusBoundary = e91;
    e91 -> mobiusBoundary = e96;

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }
    //Mobius sibling
    eX9_1 -> mobiusSibling = eX9_2;
    eX9_2 -> mobiusSibling = eX9_1;
}

void makeHild(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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

    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            }
        }
    }

    Halfedge * e16_0;
    Halfedge * e0_1;
    Halfedge * e1_10;
    Halfedge * e10_3;
    Halfedge * e3_4;
    Halfedge * e4_5;
    Halfedge * e5_6;
    Halfedge * e6_16;

    Halfedge * e15_7;
    Halfedge * e7_8;
    Halfedge * e8_9;
    Halfedge * e9_11;
    Halfedge * e11_12;
    Halfedge * e12_13;
    Halfedge * e13_14;
    Halfedge * e14_15;

    //make Boundaries
    vector<Halfedge*>::iterator eIt;
    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++){
        if((*eIt) -> start == vertices[16] && (*eIt) -> end == vertices[0]) {
            e16_0 = *eIt;
            e16_0 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[0] && (*eIt) -> end == vertices[1]) {
            e0_1 = *eIt;
            e0_1 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[1] && (*eIt) -> end == vertices[10]) {
            e1_10 = *eIt;
            e1_10 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[10] && (*eIt) -> end == vertices[3]) {
            e10_3 = *eIt;
            e10_3 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[3] && (*eIt) -> end == vertices[4]) {
            e3_4 = *eIt;
            e3_4 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[4] && (*eIt) -> end == vertices[5]) {
            e4_5 = *eIt;
            e4_5 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[5] && (*eIt) -> end == vertices[6]) {
            e5_6 = *eIt;
            e5_6 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[6] && (*eIt) -> end == vertices[16]) {
            e6_16 = *eIt;
            e6_16 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[15] && (*eIt) -> end == vertices[7]) {
            e15_7 = *eIt;
            e15_7 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[7] && (*eIt) -> end == vertices[8]) {
            e7_8 = *eIt;
            e7_8 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[8] && (*eIt) -> end == vertices[9]) {
            e8_9 = *eIt;
            e8_9 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[9] && (*eIt) -> end == vertices[11]) {
            e9_11 = *eIt;
            e9_11 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[11] && (*eIt) -> end == vertices[12]) {
            e11_12 = *eIt;
            e11_12 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[12] && (*eIt) -> end == vertices[13]) {
            e12_13 = *eIt;
            e12_13 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[13] && (*eIt) -> end == vertices[14]) {
            e13_14 = *eIt;
            e13_14 -> isSharp = true;
        }
        if((*eIt) -> start == vertices[14] && (*eIt) -> end == vertices[15]) {
            e14_15 = *eIt;
            e14_15 -> isSharp = true;
        }
    }

    //make boundaries
    e16_0 -> previousBoundary = e6_16;
    e16_0 -> nextBoundary = e0_1;

    e0_1 -> previousBoundary = e16_0;
    e0_1 -> nextBoundary = e1_10;

    e1_10 -> previousBoundary = e0_1;
    e1_10 -> nextBoundary = e10_3;

    e10_3 -> previousBoundary = e1_10;
    e10_3 -> nextBoundary = e3_4;

    e3_4 -> previousBoundary = e10_3;
    e3_4 -> nextBoundary = e4_5;

    e4_5 -> previousBoundary = e3_4;
    e4_5 -> nextBoundary = e5_6;

    e5_6 -> previousBoundary = e4_5;
    e5_6 -> nextBoundary = e6_16;

    e6_16 -> previousBoundary = e5_6;
    e6_16 -> nextBoundary = e16_0;

    e15_7 -> previousBoundary = e14_15;
    e15_7 -> nextBoundary = e7_8;

    e7_8 -> previousBoundary = e15_7;
    e7_8 -> nextBoundary = e8_9;

    e8_9 -> previousBoundary = e7_8;
    e8_9 -> nextBoundary = e9_11;

    e9_11 -> previousBoundary = e8_9;
    e9_11 -> nextBoundary = e11_12;

    e11_12 -> previousBoundary = e9_11;
    e11_12 -> nextBoundary = e12_13;

    e12_13 -> previousBoundary = e11_12;
    e12_13 -> nextBoundary = e13_14;

    e13_14 -> previousBoundary = e12_13;
    e13_14 -> nextBoundary = e14_15;

    e14_15 -> previousBoundary = e13_14;
    e14_15 -> nextBoundary = e15_7;
}

void makeWithSIF(vector<Face*> &faceVect, vector<Halfedge*> &edgeVect, vector<Vertex*> &vertVect, string inputSIF){
    vector<Face*>::iterator faceIt;
    vector<Halfedge*>::iterator edgeIt;
    Vertex * tempVert;
    Halfedge * tempEdge;
    Face * tempFace;

    //Flush the old mesh
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
    /*
    vector<Halfedge*>::iterator eIt;
    for( eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt ++) {
        if((*eIt) -> sibling == NULL) {
            cout<<"I don't have a sibling! Start from vertex "<<(*eIt) -> start -> ID<<" and I end at vertex "<<(*eIt) -> end -> ID<<endl;
        }
    }
    */
    //Siblings
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 + 1; edgeIt2 < edgeVect.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->end) && ((*edgeIt1)->end == (*edgeIt2)->start)){

                (*edgeIt1)->sibling = *edgeIt2;
                (*edgeIt2)->sibling = *edgeIt1;

            } else if (((*edgeIt1) -> start -> position == (*edgeIt2) -> start -> position) &&((*edgeIt1) -> end -> position == (*edgeIt2) -> end -> position)) {
                //cout<<"Hey, here is a mobius pair!"<<endl;
                //cout<<"One starts from vertex "<<(*edgeIt1) -> start -> ID<<" and it ends at vertex "<<(*edgeIt1) -> end -> ID<<endl;
                //cout<<"Another starts from vertex "<<(*edgeIt2) -> start -> ID<<" and it ends at vertex "<<(*edgeIt2) -> end -> ID<<endl;

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

// Initiate the mesh for OpenGL to render.
void init(int level);

void init(int level, string inputSIF);

void init(int level){
    makeCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makePyramid(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeSharpOctahedron(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeOctahedron(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeOpenCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeRing(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeSharpCube(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeMobius(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //makeHild(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect);
    //cout<< glMesh.FaceVect.size()<<" "<<glMesh.EdgeVect.size()<<" "<<glMesh.VertVect.size();
    //computeNormals(glMesh.VertVect);
    //ccSubDivision();
    Subdivision myCC;
    myCC.myMesh = glMesh;
    myCC.ccSubdivision(level);
    glMesh = myCC.myMesh;
}

void init(int level, string inputSIF){
    makeWithSIF(glMesh.FaceVect, glMesh.EdgeVect, glMesh.VertVect, inputSIF);
    //cout<<glMesh.FaceVect.size()<<" "<< glMesh.VertVect.size()<<endl;
    Subdivision myCC;
    myCC.myMesh = glMesh;
    myCC.ccSubdivision(level);
    glMesh = myCC.myMesh;
}
//************************************************************
//          OpenGL Display Functions
//************************************************************
void initRendering();

void render(void);

void reshape(int w, int h);

void keyboard(unsigned char c, int x, int y);

void keySpecial(int key, int x, int y);

void mousePressed(int button, int state, int x, int y);

void mouseMoved(int x, int y);

void initRendering(){

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient0[] = { 0.0, 0.0, 0.0, 10.0 };
    GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 10.0 };
    GLfloat light_specular0[] = { 1.0, 1.0, 1.0, 10.0 };
    GLfloat light_position0[] = { 0, 0, 1, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    glEnable(GL_LIGHT1);

    GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position1[] = { 0, 1, -0.5, 0.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHT2);

    GLfloat light_ambient2[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position2[] = { 0.877, -0.5, -0.5, 0.0 };

    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

    glEnable(GL_LIGHT3);

    GLfloat light_ambient3[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse3[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular3[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position3[] = { -0.877, -0.5, -0.5, 0.0 };

    glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse3);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular3);
    glLightfv(GL_LIGHT3, GL_POSITION, light_position3);

    GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat yello[] = {.8f, .8f, 0.f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, yello);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    GLfloat shininess[] = {50};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(5, 0, 0, 0, 0, 0, 0, 0, 1);   //  eye position, aim point, up direction
    vector<Face*>::iterator dispFaceIt;
    Face * tempFace;
    angle += 0.1;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 0, 0, 1);
    for(dispFaceIt = glMesh.FaceVect.begin(); dispFaceIt < glMesh.FaceVect.end(); dispFaceIt++){
        tempFace = *dispFaceIt;
        Vertex * tempv;
        vector<Vertex*>::iterator vIt;
        vector<Vertex*> vertices = tempFace -> vertices;
        glBegin(GL_POLYGON);
        for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
            tempv = *vIt;
            float normx = tempv -> position[0];
            float normy = tempv -> position[1];
            float normz = tempv -> position[2];
            glNormal3f(normx, normy, normz);
            float x = tempv -> position[0];
            float y = tempv -> position[1];
            float z = tempv -> position[2];
            //cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<endl;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h==0) {h=1;} // prevent a division by zero when setting aspect ratio
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    gluPerspective(45, (float)w/h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myFrameMove() {
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
    if (key == 's') {
        if (smoothShading) {
            glShadeModel(GL_FLAT);
            smoothShading = false;
        } else {
            glShadeModel(GL_SMOOTH);
            smoothShading = true;
        }
    }
    if (key == 'w') {
        if (wireframe) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            wireframe = false;
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            wireframe = true;
        }
    }
    if (key == 'l') {
        if (lightOn) {
            glDisable(GL_LIGHTING);
            lightOn = false;
        } else {
            glEnable(GL_LIGHTING);
            lightOn = true;
        }
    }
    glutPostRedisplay();
}

void keySpecial(int key, int x, int y) {
    bool shiftDown = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
    glutPostRedisplay();
}

void mousePressed(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        exit(0);
    }
}

void mouseMoved(int x, int y){
    
}

//************************************************************
//          Main Function
//************************************************************

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    viewport.width = 640;
    viewport.hight = 480;

    //Process the command line arguments
    int level;
    string inputSIF;
    if(argc == 1 || argc > 3){
        cout<<"USAGE: ./NAME_OF_PROGRAM LEVEL_OF_SUBDIVISION INPUT_SIF_FILE.";
        exit(1);
    } else if(argc == 2){
        level = stoi(argv[1]);
        init(level);
    } else if(argc == 3){
        level = stoi(argv[1]);
        inputSIF = argv[2];
        init(level, inputSIF);
    }
    glutInitWindowSize(viewport.width, viewport.hight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    initRendering();
    glutDisplayFunc(render);
    // General UI functions
    glutReshapeFunc(reshape);
    glutIdleFunc(myFrameMove); 
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keySpecial);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mouseMoved);

    glutMainLoop();
}


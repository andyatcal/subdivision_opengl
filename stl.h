// 
// @author Andy Wang, UC Berkeley. Copyright 2015 reserve.
// Catmull-Clark subdivision project in order to create sculptures like Eva-Hilds.
// Advised by Prof. Sequin H. Carlos.
// 
//

#ifndef __STL_H__
#define __STL_H__

#include <glm/glm.hpp>
#include <vector>
#include <iomanip>

using namespace glm;
using namespace std;

// Forward declarations
class Vertex;
class Face;

//////////////////////////////////////////////////////////////////////
// STL Class -- Input and Output with STL Format Files.
//
class STL{
public:
    STL(){};
    void STLOutput(vector<Mesh> &meshes, string outputSTL);
};

void STL::STLOutput(vector<Mesh> &meshes, string outputSTL){
    vector<Mesh>::iterator mIt;
    vector<Face*>::iterator faceIt;

    ofstream file(outputSTL);
    if (!file.is_open()) {
        cout <<"COULD NOT OPEN THE FILE.\n";
    } else {
        file << "solid\n";
        for(mIt = meshes.begin(); mIt < meshes.end(); mIt++) {
            Mesh & currMesh = (*mIt);
            vector<Face*> &faceVect = currMesh.faceVect;
            vector<Face*>::iterator fIt;
            for(fIt = faceVect.begin(); fIt < faceVect.end(); fIt++) {
                Halfedge * firstSideEdge = (*fIt) -> oneSideEdge;
                if(firstSideEdge == NULL) {
                    cout<<"ERROR: This face (with ID)" <<(*fIt) -> ID << "does not have a sideEdge."<<endl;
                    exit(0);
                }
                Vertex * v0 = firstSideEdge -> start;
                vec3 p0 = v0 -> position;
                Halfedge * nextSideEdge = firstSideEdge -> next;
                if(nextSideEdge == NULL) {
                    cout<<"ERROR: This face (with ID)" <<(*fIt) -> ID << "contains only one edge and can not be drawn."<<endl;
                }
                do {
                    Vertex * v1 = nextSideEdge -> start;
                    Vertex * v2 = nextSideEdge -> end;
                    Mesh tempMesh;
                    makeTriFace(v0, v1, v2, tempMesh.faceVect, tempMesh.edgeVect);
                    getFaceNormal(tempMesh.faceVect[0]);
                    vec3 faceNormal = tempMesh.faceVect[0] -> faceNormal;
                    file << "  facet normal "<<faceNormal[0]<<" "<<faceNormal[1]<<" "<<faceNormal[2]<<"\n";
                    file << "    outer loop\n";
                    vec3 p1 = v1 -> position;
                    vec3 p2 = v2 -> position;
                    file << "      vertex " << p0[0] << " "<< p0[1] << " " << p0[2]<<"\n";
                    file << "      vertex " << p1[0] << " "<< p1[1] << " " << p1[2]<<"\n";
                    file << "      vertex " << p2[0] << " "<< p2[1] << " " << p2[2]<<"\n";
                    file << "    endloop\n";
                    file << "  endfacet\n";
                    nextSideEdge = nextSideEdge -> next;
                } while (nextSideEdge -> next != firstSideEdge);
            }
        }
        file << "endsolid\n";
    }
}
#endif// __STL_H__
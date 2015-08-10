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
    void STLOutput(vector<Mesh> meshes, string outputSTL);
};

void STL::STLOutput(vector<Mesh> meshes, string outputSTL){
    /*
    for(Mesh mesh :: meshes) {

    }
    */
    cout<<"Hello!"<<endl;
    vector<Face*>::iterator faceIt;

    ofstream file(outputSTL);
    if (!file.is_open()) {
        cout <<"COULD NOT OPEN THE FILE.";
    } else {
        file << "This is a line.\n";
        file << "This is another line.\n";
        file.close();
    }
}
#endif// __STL_H__
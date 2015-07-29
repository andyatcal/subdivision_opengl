
#include <GL/glut.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
//#include <glm/glm.hpp>
#include <vector>
#include <stb_image.c>
#include <glext.h>
#define PI 3.1415926535897932384626433832795028841971693993751058209

using namespace std;
//globals for looking at I
float xrot, yrot;
float zoom = -6.0;


//forward class declarations
class vec;
class vertex;
class face;
class halfEdge;


bool checkCC;
bool checkFlush;
bool showMesh;

//this class is for a vector data type
class vec{
public:
	float x, y, z;

};

//this class is for a vertex data type
class vertex{
public:
	vertex::vertex();
	double x, y, z;
	halfEdge * oneOutEdge;
	vec normal;

};

//this class stores a face
class face{
public:
	face::face();
	vertex *v1;
	vertex *v2;
	vertex *v3; 
	vertex *v4;
	halfEdge * oneSideEdge; 
	vertex * facePoint;
};

class halfEdge{
public:
	halfEdge::halfEdge();
	vertex * start;
	vertex * end; 
	halfEdge * next; 
	halfEdge * opposite;
	face * heFace; 

	vertex* edgePoint;
	halfEdge* firstHalf;
	halfEdge* secondHalf;

};

//constructors
halfEdge::halfEdge(){
	start = NULL;
	end = NULL;
	next = NULL;
	opposite = NULL;
	heFace = NULL;
	edgePoint = NULL;
	firstHalf = NULL;
	secondHalf = NULL;


}

face::face(){
	oneSideEdge = NULL;
	facePoint = NULL;
	v1 = v2 = v3 = v4 = NULL;


}


vertex::vertex(){

	oneOutEdge = NULL;
	x = y = z = 0;

}

//bezier control points
vertex cntlPt0;
vertex cntlPt1;
vertex cntlPt2;
vertex cntlPt3;
double t;
bool dirswitch;

//for textures
GLuint texNames[2];

//global mesh vectors
std::vector<halfEdge*> glEdgeVect;
std::vector<vertex*> glVertVect;
std::vector<face*> glFaceVect;


//returns the magnitude of the vector input
float magnitude(float x, float y, float z){
	return sqrt(x*x + y*y + z*z);
	
}

/*given a vector, this function will unitize it
*/
void unitize(vec & inVec){

	float currx = inVec.x;
	float curry = inVec.y;
	float currz = inVec.z;
	
	float length = magnitude(currx, curry, currz);


	inVec.x = currx/length;
	inVec.y = curry/length;
	inVec.z = currz/length;


}




/*this will compute the cross product of 2 vectors, and put that resulting vector into 
	a third vector passed in as an argument via pointers.
*/
void xprod(float ax, float ay, float az, float bx, float by, float bz, vec &inVec){

	
	inVec.x = (ay* bz) - (by*az);
	inVec.y = (az*bx) - (bz*ax);
	inVec.z = (ax*by) - (bx* ay);



}

/*goes through every face, and creates a face vertex and points that faces facePoint vector to its face point
*/
void makeFacePoints(std::vector<face*> &faceVect, std::vector<vertex*> &vertVect){
	std::vector<face*>::iterator it;
	
	for(it = faceVect.begin(); it < faceVect.end(); it++){

		//load the verts of this face
		vertex vert1 = *((*it)->v1);
		vertex vert2 = *((*it)->v2);
		vertex vert3 = *((*it)->v3);
		vertex vert4 = *((*it)->v4);

		//make new vertex that is average of this faces verts
		vertex * newFacePoint = new vertex;

		(*newFacePoint).x = (vert1.x + vert2.x + vert3.x + vert4.x)/4;
		(*newFacePoint).y = (vert1.y + vert2.y + vert3.y + vert4.y)/4;
		(*newFacePoint).z = (vert1.z + vert2.z + vert3.z + vert4.z)/4;

		vertVect.push_back(newFacePoint);
		(*it)->facePoint = newFacePoint;



	}
}

//construct all of the edge points, and store in existing edges
void makeEdgePoints(std::vector<halfEdge*> &HEvect, std::vector<vertex*> &vertVect){
	std::vector<halfEdge*>::iterator it;

	for(it = HEvect.begin(); it < HEvect.end(); it++){
		if( (*it)->opposite->edgePoint != NULL){
			(*it)->edgePoint = (*it)->opposite->edgePoint;
			
		}
		else{
			vertex faceVert1 = *((*it)->heFace->facePoint);
			vertex faceVert2 = *((*it)->opposite->heFace->facePoint);
			vertex edgeVert1 = *((*it)->end);
			vertex edgeVert2 = *((*it)->opposite->end);

			vertex * newEdgeVertex = new vertex;
			//average other stuff here

			(*newEdgeVertex).x = (faceVert1.x + faceVert2.x + edgeVert1.x + edgeVert2.x)/4;
			(*newEdgeVertex).y = (faceVert1.y + faceVert2.y + edgeVert1.y + edgeVert2.y)/4;
			(*newEdgeVertex).z = (faceVert1.z + faceVert2.z + edgeVert1.z + edgeVert2.z)/4;

			vertVect.push_back(newEdgeVertex);
			(*it)->edgePoint = newEdgeVertex;

		}

	}

}


//adjust all of the old vertex points based on the new edge and face points that were calculated above. 
void makeAdjustedVerts(std::vector<vertex*> & vertVect){
	std::vector<vertex*>::iterator it;

	halfEdge* edge1;
	halfEdge* edge2;
	halfEdge* edge3;
	halfEdge* edge4;
	halfEdge* edge5;

	vertex facePoint1;
	vertex facePoint2;
	vertex facePoint3;
	vertex facePoint4;
	vertex facePoint5;
	vertex facePointAvg;

	vertex edgePoint1;
	vertex edgePoint2;
	vertex edgePoint3;
	vertex edgePoint4;
	vertex edgePoint5;
	vertex edgePointAvg;

	vertex * currVert;
	
	for(it = vertVect.begin(); it < vertVect.end(); it ++){
		currVert = *it;
		//check if vertex has been checked already
		
		edge1 = currVert->oneOutEdge;
		edge2 = edge1->next->next->next->opposite;
		edge3 = edge2->next->next->next->opposite;
		edge4 = edge3->next->next->next->opposite;
		edge5 = edge4->next->next->next->opposite;


		edgePoint1 = *(edge1->edgePoint); 
		edgePoint2 = *(edge2->edgePoint); 
		edgePoint3 = *(edge3->edgePoint); 
		edgePoint4 = *(edge4->edgePoint); 
		edgePoint5 = *(edge5->edgePoint); 


		facePoint1 = *(edge1->heFace->facePoint);
		facePoint2 = *(edge2->heFace->facePoint);
		facePoint3 = *(edge3->heFace->facePoint);
		facePoint4 = *(edge4->heFace->facePoint);
		facePoint5 = *(edge5->heFace->facePoint);

		int divnum,multnum;
		if(edge1 == edge4){
			//average the face points and the edge points
			edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x)/3;
			edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y)/3;
			edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z)/3;

			facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x)/3;
			facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y)/3;
			facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z)/3;
			divnum =4;
			multnum = 1;
		}
		else if(edge1 == edge5){
			//average the face points and the edge points
			edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x)/4;
			edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y)/4;
			edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z)/4;

			facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x)/4;
			facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y)/4;
			facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z)/4;
			divnum =4;
			multnum = 1;
		}
		else{
				//average the face points and the edge points
			edgePointAvg.x = (edgePoint1.x + edgePoint2.x + edgePoint3.x + edgePoint4.x + edgePoint5.x)/5;
			edgePointAvg.y = (edgePoint1.y + edgePoint2.y + edgePoint3.y + edgePoint4.y + edgePoint5.y)/5;
			edgePointAvg.z = (edgePoint1.z + edgePoint2.z + edgePoint3.z + edgePoint4.z + edgePoint5.z)/5;

			facePointAvg.x = (facePoint1.x + facePoint2.x + facePoint3.x + facePoint4.x + facePoint5.x)/5;
			facePointAvg.y = (facePoint1.y + facePoint2.y + facePoint3.y + facePoint4.y + facePoint5.y)/5;
			facePointAvg.z = (facePoint1.z + facePoint2.z + facePoint3.z + facePoint4.z + facePoint5.z)/5;
			divnum =5;
			multnum = 2;
		}



		

		//update the current vector based off of averages just computed 
		currVert->x = (multnum*currVert->x +  2*edgePointAvg.x + facePointAvg.x)/divnum; 
		currVert->y = (multnum*currVert->y +  2*edgePointAvg.y + facePointAvg.y)/divnum; 
		currVert->z = (multnum*currVert->z +  2*edgePointAvg.z + facePointAvg.z)/divnum; 


	}



}


//take all of the new edge and face points, as well as the adjusted old vertices, and connect them together with
//a new set of faces and edges
void compileNewMesh(std::vector<face*> &faceVect, std::vector<face*> &newFaceVect, std::vector<halfEdge*> &newEdgeVect){
	std::vector<face*>::iterator faceIt;

	for(faceIt = faceVect.begin(); faceIt < faceVect.end(); faceIt ++){
		face currFace = **faceIt;

		halfEdge* edge1 = currFace.oneSideEdge;
		halfEdge *edge2 = edge1->next;
		halfEdge *edge3 = edge2->next;
		halfEdge *edge4 = edge3->next;

		halfEdge* edge1a = new halfEdge;
		halfEdge* edge1b = new halfEdge;
		halfEdge* edge2a = new halfEdge;
		halfEdge* edge2b = new halfEdge;
		halfEdge* edge3a = new halfEdge;
		halfEdge* edge3b = new halfEdge;
		halfEdge* edge4a = new halfEdge;
		halfEdge* edge4b = new halfEdge;

		halfEdge* edge1in = new halfEdge;
		halfEdge* edge1out = new halfEdge;
		halfEdge* edge2in = new halfEdge;
		halfEdge* edge2out = new halfEdge;
		halfEdge* edge3in = new halfEdge;
		halfEdge* edge3out = new halfEdge;
		halfEdge* edge4in = new halfEdge;
		halfEdge* edge4out = new halfEdge;


		//construct border edges 
		edge1->firstHalf = edge1a; 
		edge1->secondHalf = edge1b;
		edge2->firstHalf = edge2a; 
		edge2->secondHalf = edge2b;
		edge3->firstHalf = edge3a; 
		edge3->secondHalf = edge3b;
		edge4->firstHalf = edge4a; 
		edge4->secondHalf = edge4b;

		//replace edge1
		edge1a->start = edge1->start;
		edge1a->end = edge1->edgePoint;
		edge1b->start = edge1->edgePoint;
		edge1b->end = edge1->end; 
		if(edge1->opposite->firstHalf != NULL){
			edge1b->opposite = edge1->opposite->firstHalf;
			edge1a->opposite = edge1->opposite->secondHalf;
			
			edge1->opposite->firstHalf->opposite = edge1b;
			edge1->opposite->secondHalf->opposite = edge1a;
		}
		//adjust edge pointer from start vertex
		edge1->start->oneOutEdge = edge1a;
		//inner edge from edge 1
		edge1in->start = edge1->edgePoint;
		edge1in->end = currFace.facePoint;
		edge1out->end = edge1->edgePoint;
		edge1out->start = currFace.facePoint;
		edge1in->opposite = edge1out;
		edge1out->opposite = edge1in;
		//set edge pointer from edge1 vertex
		edge1->edgePoint->oneOutEdge = edge1in;


		//replace edge2
		edge2a->start = edge2->start;
		edge2a->end = edge2->edgePoint;
		edge2b->start = edge2->edgePoint;
		edge2b->end = edge2->end; 
		if(edge2->opposite->firstHalf != NULL){
			edge2b->opposite = edge2->opposite->firstHalf;
			edge2a->opposite = edge2->opposite->secondHalf;
			edge2->opposite->firstHalf->opposite = edge2b;
			edge2->opposite->secondHalf->opposite = edge2a;
		}
		//adjust edge pointer from start vertex
		edge2->start->oneOutEdge = edge2a;
		//inner edge from edge 2
		edge2in->start = edge2->edgePoint;
		edge2in->end = currFace.facePoint;
		edge2out->end = edge2->edgePoint;
		edge2out->start = currFace.facePoint;
		edge2in->opposite = edge2out;
		edge2out->opposite = edge2in;
		//set edge pointer from edge2 vertex
		edge2->edgePoint->oneOutEdge = edge2in;



		//replace edge3
		edge3a->start = edge3->start;
		edge3a->end = edge3->edgePoint;
		edge3b->start = edge3->edgePoint;
		edge3b->end = edge3->end; 
		if(edge3->opposite->firstHalf != NULL){
			edge3b->opposite = edge3->opposite->firstHalf;
			edge3a->opposite = edge3->opposite->secondHalf;
			edge3->opposite->firstHalf->opposite = edge3b;
			edge3->opposite->secondHalf->opposite = edge3a;
		}
		//adjust edge pointer from start vertex
		edge2->start->oneOutEdge = edge2a;
		//inner edge from edge 3
		edge3in->start = edge3->edgePoint;
		edge3in->end = currFace.facePoint;
		edge3out->end = edge3->edgePoint;
		edge3out->start = currFace.facePoint;
		edge3in->opposite = edge3out;
		edge3out->opposite = edge3in;
		//set edge pointer from edge3 vertex
		edge3->edgePoint->oneOutEdge = edge3in;



		//replace edge4
		edge4a->start = edge4->start;
		edge4a->end = edge4->edgePoint;
		edge4b->start = edge4->edgePoint;
		edge4b->end = edge4->end; 
		if(edge4->opposite->firstHalf != NULL){
			edge4b->opposite = edge4->opposite->firstHalf;
			edge4a->opposite = edge4->opposite->secondHalf;
			//modify so that BOTH sets of half edges have
			edge4->opposite->firstHalf->opposite = edge4b;
			edge4->opposite->secondHalf->opposite = edge4a;

		}
		//adjust edge pointer from start vertex
		edge4->start->oneOutEdge = edge4a;
		//inner edge from edge 4
		edge4in->start = edge4->edgePoint;
		edge4in->end = currFace.facePoint;
		edge4out->end = edge4->edgePoint;
		edge4out->start = currFace.facePoint;
		edge4in->opposite = edge4out;
		edge4out->opposite = edge4in;
		//set edge pointer from edge4 vertex
		edge4->edgePoint->oneOutEdge = edge4in;


		//set face vertex edge pointer
		currFace.facePoint->oneOutEdge = edge4out;

		//set all next pointers
		edge1a->next = edge1in;
		edge1in->next = edge4out;
		edge4out->next = edge4b; 
		edge4b->next = edge1a;

		edge2a->next = edge2in;
		edge2in->next = edge1out;
		edge1out->next = edge1b; 
		edge1b->next = edge2a;

		edge3a->next = edge3in;
		edge3in->next = edge2out;
		edge2out->next = edge2b; 
		edge2b->next = edge3a;

		edge4a->next = edge4in;
		edge4in->next = edge3out;
		edge3out->next = edge3b; 
		edge3b->next = edge4a;
			

		//create new faces/connect all face pointers
		face *face1 = new face;
		face1->v1 = edge1a->start;
		edge1a->heFace = face1;
		face1->v2 = edge1in->start;
		edge1in->heFace = face1;
		face1->v3 = edge4out->start;
		edge4out->heFace = face1;
		face1->v4 = edge4b->start;
		edge4b->heFace = face1;
		face1->oneSideEdge = edge4b;

		face *face2 = new face;
		face2->v1 = edge2a->start;
		edge2a->heFace = face2;
		face2->v2 = edge2in->start;
		edge2in->heFace = face2;
		face2->v3 = edge1out->start;
		edge1out->heFace = face2;
		face2->v4 = edge1b->start;
		edge1b->heFace = face2;
		face2->oneSideEdge = edge1b;


		face *face3 = new face;
		face3->v1 = edge3a->start;
		edge3a->heFace = face3;
		face3->v2 = edge3in->start;
		edge3in->heFace = face3;
		face3->v3 = edge2out->start;
		edge2out->heFace = face3;
		face3->v4 = edge2b->start;
		edge2b->heFace = face3;
		face3->oneSideEdge = edge2b;


		face *face4 = new face;
		face4->v1 = edge4a->start;
		edge4a->heFace = face4;
		face4->v2 = edge4in->start;
		edge4in->heFace = face4;
		face4->v3 = edge3out->start;
		edge3out->heFace = face4;
		face4->v4 = edge3b->start;
		edge3b->heFace = face4;
		face4->oneSideEdge = edge3b;



		//load each face into the new face vector
		newFaceVect.push_back(face1); 
		newFaceVect.push_back(face2); 
		newFaceVect.push_back(face3); 
		newFaceVect.push_back(face4); 

		//push all new edges onto the new edge vector
		newEdgeVect.push_back(edge1a);
		newEdgeVect.push_back(edge1b);
		newEdgeVect.push_back(edge2a);
		newEdgeVect.push_back(edge2b);
		newEdgeVect.push_back(edge3a);
		newEdgeVect.push_back(edge3b);
		newEdgeVect.push_back(edge4a);
		newEdgeVect.push_back(edge4b);

		newEdgeVect.push_back(edge1in);
		newEdgeVect.push_back(edge1out);
		newEdgeVect.push_back(edge2in);
		newEdgeVect.push_back(edge2out);
		newEdgeVect.push_back(edge3in);
		newEdgeVect.push_back(edge3out);
		newEdgeVect.push_back(edge4in);
		newEdgeVect.push_back(edge4out);

	}





}

//compute the normal vector of the vertex at the start of the input half edge
vec getNormal(halfEdge currEdge){
	vertex v1 = *(currEdge.start);
	vertex v2 = *(currEdge.end);
	vertex v3 = *((currEdge.next)->end);

	vec outNorm;
	
	xprod((v2.x-v1.x),(v2.y-v1.y),(v2.z-v1.z),(v3.x-v2.x),(v3.y-v2.y),(v3.z-v2.z), outNorm);

	return outNorm;
}

//iterate over every vertex in the mesh and compute its normal
void computeNormals(std::vector<vertex*> &vertVect){
	std::vector<vertex*>::iterator vIt;

	halfEdge* edge1;
	halfEdge* edge2;
	halfEdge* edge3;
	halfEdge* edge4;
	halfEdge* edge5;

	vec norm1;
	vec norm2;
	vec norm3;
	vec norm4;
	vec norm5;
	vec avgNorm;

	vertex * currVert;

	for(vIt =vertVect.begin(); vIt < vertVect.end(); vIt ++){
		currVert = *vIt;
		//check if vertex has been checked already
		
		edge1 = currVert->oneOutEdge;
		edge2 = edge1->next->next->next->opposite;
		edge3 = edge2->next->next->next->opposite;
		edge4 = edge3->next->next->next->opposite;
		edge5 = edge4->next->next->next->opposite;

		norm1 = getNormal(*edge1);
		norm2 = getNormal(*edge2);
		norm3 = getNormal(*edge3);
		norm4 = getNormal(*edge4);
		norm5 = getNormal(*edge5);


		
		if(edge1 == edge4){
			avgNorm.x =  (norm1.x + norm2.x + norm3.x)/3;
			avgNorm.y =  (norm1.y + norm2.y + norm3.y)/3;
			avgNorm.z =  (norm1.z + norm2.z + norm3.z)/3;



		}
		else if(edge1 == edge5){
			//average the face points and the edge points
			avgNorm.x =  (norm1.x + norm2.x + norm3.x + norm4.x)/4;
			avgNorm.y =  (norm1.y + norm2.y + norm3.y + norm4.y)/4;
			avgNorm.z =  (norm1.z + norm2.z + norm3.z + norm4.z)/4;



		}
		else{
			avgNorm.x =  (norm1.x + norm2.x + norm3.x + norm4.x + norm5.x)/5;
			avgNorm.y =  (norm1.y + norm2.y + norm3.y + norm4.y + norm5.y)/5;
			avgNorm.z =  (norm1.z + norm2.z + norm3.z + norm4.z + norm5.z)/5;

			
		}
		/*
		avgNorm.x *= -1;
		avgNorm.y *= -1;
		avgNorm.z *= -1;
		*/

		unitize(avgNorm);
		currVert->normal = avgNorm;

	}


}


/* this function is the overall catmull-clark function, responsible for calling all of the above functions in sequence
to create the new subdivided mesh*/
void fullCC(){

	std::vector<face*> nFaceVect;
	std::vector<halfEdge*> nEdgeVect;
	std::vector<vertex*> oldVertVect;


	halfEdge * tempEdge;
	face * tempFace;

	oldVertVect = glVertVect;
	makeFacePoints(glFaceVect, glVertVect);

	makeEdgePoints(glEdgeVect, glVertVect);

	makeAdjustedVerts(oldVertVect);

	compileNewMesh(glFaceVect, nFaceVect, nEdgeVect); 
	computeNormals(glVertVect);

	//delete all old edges and faces
	while(!glFaceVect.empty()){
		tempFace = glFaceVect.back();
		glFaceVect.pop_back();
		delete tempFace;

	}

	while(!glEdgeVect.empty()){
		tempEdge = glEdgeVect.back();
		glEdgeVect.pop_back();
		delete tempEdge;

	}


	glFaceVect = nFaceVect;
	glEdgeVect = nEdgeVect;


}


//this function is called below as part of the creation of the initial block I
void initMakeFace(vertex * va, vertex* vb, vertex * vc, vertex * vd, 
				std::vector<face*> &faceVect, 
				std::vector<halfEdge*> &edgeVect){
	face * nextFace = new face;
	halfEdge *e1, *e2, *e3, *e4;

	e1 = new halfEdge;
	e2 = new halfEdge;
	e3 = new halfEdge;
	e4 = new halfEdge;

	//set face's vertex pointers
	nextFace->v1 = va;
	nextFace->v2 = vb;
	nextFace->v3 = vc;
	nextFace->v4 = vd;

	//set start and end pointers
	e1->start = va;
	e2->start = vb;
	e3->start = vc;
	e4->start = vd;
	e1->end = vb;
	e2->end = vc;
	e3->end = vd;
	e4->end = va;

	//set oneOutEdge pointers
	va->oneOutEdge = e1;
	vb->oneOutEdge = e2;
	vc->oneOutEdge = e3;
	vd->oneOutEdge = e4;


	//set next pointers
	e1->next = e2;
	e2->next = e3;
	e3->next = e4;
	e4->next = e1;
	
	//set face pointers
	e1->heFace = nextFace;
	e2->heFace = nextFace;
	e3->heFace = nextFace;
	e4->heFace = nextFace;

	//set faces edge pointer
	nextFace->oneSideEdge = e4;


	//push everything 
	faceVect.push_back(nextFace);
	edgeVect.push_back(e1);
	edgeVect.push_back(e2);
	edgeVect.push_back(e3);
	edgeVect.push_back(e4);
	

}

//this function deletes the current mesh being displayed(if any) and creates a new, un subdivided block I to display
void flushAndReset(std::vector<face*> &faceVect, std::vector<halfEdge*> &edgeVect, std::vector<vertex*> &vertVect){
	std::vector<face*>::iterator faceIt;
	std::vector<halfEdge*>::iterator edgeIt;
	vertex * tempVert;
	halfEdge * tempEdge;
	face * tempFace;
	//clean out old mesh
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
	vertex * v1 = new vertex;
	vertex * v2 = new vertex;
	vertex * v3 = new vertex;
	vertex * v4 = new vertex;
	vertex * v5 = new vertex;
	vertex * v6 = new vertex;
	vertex * v7 = new vertex;
	vertex * v8 = new vertex;
	vertex * v9 = new vertex;
	vertex * v10 = new vertex;
	vertex * v11 = new vertex;
	vertex * v12 = new vertex;
	vertex * v13 = new vertex;
	vertex * v14 = new vertex;
	vertex * v15 = new vertex;
	vertex * v16 = new vertex;
	vertex * v17 = new vertex;
	vertex * v18 = new vertex;
	vertex * v19 = new vertex;
	vertex * v20 = new vertex;
	vertex * v21 = new vertex;
	vertex * v22 = new vertex;
	vertex * v23 = new vertex;
	vertex * v24 = new vertex;
	vertex * v25 = new vertex;
	vertex * v26 = new vertex;
	vertex * v27 = new vertex;
	vertex * v28 = new vertex;
	vertex * v29 = new vertex;
	vertex * v30 = new vertex;
	vertex * v31 = new vertex;
	vertex * v32 = new vertex;

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
	vertVect.push_back(v10);
	vertVect.push_back(v11);
	vertVect.push_back(v12);
	vertVect.push_back(v13);
	vertVect.push_back(v14);
	vertVect.push_back(v15);
	vertVect.push_back(v16);
	vertVect.push_back(v17);
	vertVect.push_back(v18);
	vertVect.push_back(v19);
	vertVect.push_back(v20);
	vertVect.push_back(v21);
	vertVect.push_back(v22);
	vertVect.push_back(v23);
	vertVect.push_back(v24);
	vertVect.push_back(v25);
	vertVect.push_back(v26);
	vertVect.push_back(v27);
	vertVect.push_back(v28);
	vertVect.push_back(v29);
	vertVect.push_back(v30);
	vertVect.push_back(v31);
	vertVect.push_back(v32);



	//set vertex coordinates 
	//set all x coords
	v1->x = v5->x = v9->x = v13->x = v17->x = v21->x = v25->x = v29->x = -1; 
	v2->x = v6->x = v10->x = v14->x = v18->x = v22->x = v26->x = v30->x = -.2; 
	v3->x = v7->x = v11->x = v15->x = v19->x = v23->x = v27->x = v31->x = .2; 
	v4->x = v8->x = v12->x = v16->x = v20->x = v24->x = v28->x = v32->x = 1; 


	//set all y coords
	v1->y = v2->y = v3->y = v4->y = v17->y = v18->y = v19->y = v20->y = 1; 
	v5->y = v6->y = v7->y = v8->y = v21->y = v22->y = v23->y = v24->y = .6; 
	v9->y = v10->y = v11->y = v12->y = v25->y = v26->y = v27->y = v28->y = -.6; 
	v13->y = v14->y = v15->y = v16->y = v29->y = v30->y = v31->y = v32->y = -1; 

	//set all z coords
	v1->z = v2->z = v3->z = v4->z = v5->z = v6->z = v7->z = v8->z = v9->z = v10->z = v11->z = v12->z = v13->z = v14->z = v15->z = v16->z =.2;
	v17->z = v18->z = v19->z = v20->z = v21->z = v22->z = v23->z = v24->z = v25->z = v26->z = v27->z = v28->z = v29->z = v30->z = v31->z = v32->z =-.2;


	//call face constructor function for each face/quad of verts
	//front face of I
	initMakeFace(v5, v6, v2, v1, faceVect, edgeVect);
	initMakeFace(v6, v7, v3, v2, faceVect, edgeVect);
	initMakeFace(v7, v8, v4, v3, faceVect, edgeVect);
	initMakeFace(v10, v11, v7, v6, faceVect, edgeVect);
	initMakeFace(v13, v14, v10, v9, faceVect, edgeVect);
	initMakeFace(v14, v15, v11, v10, faceVect, edgeVect);
	initMakeFace(v15, v16, v12, v11, faceVect, edgeVect);
	//back face of I
	initMakeFace(v17, v18, v22, v21, faceVect, edgeVect);
	initMakeFace(v18, v19, v23, v22, faceVect, edgeVect);
	initMakeFace(v19, v20, v24, v23, faceVect, edgeVect);
	initMakeFace(v22, v23, v27, v26, faceVect, edgeVect);
	initMakeFace(v25, v26, v30, v29, faceVect, edgeVect);
	initMakeFace(v26, v27, v31, v30, faceVect, edgeVect);
	initMakeFace(v27, v28, v32, v31, faceVect, edgeVect);
	//top face
	initMakeFace(v18, v17, v1, v2, faceVect, edgeVect);
	initMakeFace(v19, v18, v2, v3, faceVect, edgeVect);
	initMakeFace(v20, v19, v3, v4, faceVect, edgeVect);
	//bottom face
	initMakeFace(v14, v13, v29, v30, faceVect, edgeVect);
	initMakeFace(v15, v14, v30, v31, faceVect, edgeVect);
	initMakeFace(v16, v15, v31, v32, faceVect, edgeVect);
	//in-top faces
	initMakeFace(v6, v5, v21, v22, faceVect, edgeVect);
	initMakeFace(v8, v7, v23, v24, faceVect, edgeVect);
	//in-bottom faces
	initMakeFace(v9, v10, v26, v25, faceVect, edgeVect);
	initMakeFace(v11, v12, v28, v27, faceVect, edgeVect);
	//right side
	initMakeFace(v4, v8, v24, v20, faceVect, edgeVect);
	initMakeFace(v12, v16, v32, v28, faceVect, edgeVect);
	initMakeFace(v7, v11, v27, v23, faceVect, edgeVect);
	//left side
	initMakeFace(v17, v21, v5, v1, faceVect, edgeVect);
	initMakeFace(v22, v26, v10, v6, faceVect, edgeVect);
	initMakeFace(v25, v29, v13, v9, faceVect, edgeVect);

		
	std::vector<halfEdge*>::iterator edgeIt1;
	std::vector<halfEdge*>::iterator edgeIt2;
	

	for( edgeIt1 = edgeVect.begin(); edgeIt1 < edgeVect.end(); edgeIt1 ++){
		for(edgeIt2 = edgeIt1 +1; edgeIt2 < edgeVect.end(); edgeIt2++){
			if(((*edgeIt1)->start == (*edgeIt2)->end) &&((*edgeIt1)->end == (*edgeIt2)->start)){
		
				(*edgeIt1)->opposite = *edgeIt2;
				(*edgeIt2)->opposite = *edgeIt1;
				
			}



		}




	}

	 

}


void init(void) 
{
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat purple[] = {0.5, 0.11, 0.87};
	//GLfloat lpos[] = {0.0, 0.0, -5.0, 0.0};
	GLfloat grey[] = {.3,.3,.3};
	GLfloat lpos[] = {6.0, 9.0, 12.0, 0.0};
	GLfloat amb[] = {0.0,0.0,0.0,1.0};
	GLfloat color[] = {.4, .2, 1.0, 1.0};
	GLfloat ambl[] = {0.2,0.2,0.2,1};
	
	
	
	//lighting stuff
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, ambl);
	glEnable(GL_LIGHTING);
	
	
	//material stuff (part of lighting stuff)/*
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(2, texNames);

	//these next 5 blocks of code is responsible for loading a different texture image from a file and putting it into video memory
	glBindTexture(GL_TEXTURE_2D, texNames[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	int imageWidth, imageHeight, imageComp;
	imageWidth = imageHeight = imageComp = 0;
	unsigned char * texture = NULL;
	texture = stbi_load(/*"C:/Users/Jeff/Documents/Visual Studio 2010/Projects/MP2/Debug/ */"rbflow.jpeg", &imageWidth, &imageHeight, &imageComp, 3);
	//load texture into viedo memory
//	texture = NULL;
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	

	//set up new mesh of the I
	flushAndReset(glFaceVect, glEdgeVect, glVertVect);
	computeNormals(glVertVect);


	//allows for specular reflection to still show up when textures are drawn
	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );

	glClearColor (0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
//	glEnable(GL_CULL_FACE);
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity ();             /* clear the matrix */

	vertex camLoc;
	if((t >=1)||(t<=0)){
		dirswitch = !dirswitch;
		
	}
	camLoc.x = pow((1-t),3)*cntlPt0.x + 3*pow((1-t),2)*t*cntlPt1.x + 3*(1-t)*pow(t,2)*cntlPt2.x +  pow(t,3)*cntlPt3.x;
	camLoc.y = pow((1-t),3)*cntlPt0.y + 3*pow((1-t),2)*t*cntlPt1.y + 3*(1-t)*pow(t,2)*cntlPt2.y +  pow(t,3)*cntlPt3.y;
	camLoc.z = pow((1-t),3)*cntlPt0.z + 3*pow((1-t),2)*t*cntlPt1.z + 3*(1-t)*pow(t,2)*cntlPt2.z +  pow(t,3)*cntlPt3.z;
	
	if(dirswitch){
		t += .002;
	}
	else{
		t-= .002;
	}

	if(checkCC == true){
		fullCC();
		checkCC = false;
	}

	if(checkFlush == true){
		flushAndReset(glFaceVect, glEdgeVect, glVertVect);
		computeNormals(glVertVect);
		checkFlush = false;
	}

//	glEnable(GL_LIGHT0);
	gluLookAt(camLoc.x, camLoc.y, camLoc.z, 0,0,0, 0,1,0);

	
	//translations and rotations to allow for zooming and rotating of teapost
/*	glTranslatef(0.0, 0.0, zoom);
//	glTranslatef(0.0, 1.0, 0.0);
	glRotatef(xrot, 1.0, 0.0, 0.0);
	glRotatef(yrot, 0.0, 1.0, 0.0);
//  glTranslatef(0.0, -1.0, 0.0);
	*/
	
//	glColor3f(0.5f, 0.11f, 0.87f);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture (GL_TEXTURE_2D, texNames[0]);

    angle += 0.5;
    if (angle > 360) {angle -= 360;}
    glRotatef(angle, 1, 1, 1);

	std::vector<face*>::iterator dispFaceIt;
	face* tempFace;
	for(dispFaceIt = glFaceVect.begin(); dispFaceIt < glFaceVect.end(); dispFaceIt ++){
		tempFace = *dispFaceIt;
		vertex *tempv1 = tempFace->v1;
		vertex *tempv2 = tempFace->v2;
		vertex *tempv3 = tempFace->v3;
		vertex *tempv4 = tempFace->v4;

		//cross product of (v2-v1)x(v3-v2)

		if(showMesh){
			glDisable(GL_TEXTURE_2D);
		//	glDisable(GL_LIGHT0);
			glColor3f(1.0f,1.0f,1.0f);
			glBegin(GL_LINE_LOOP);
				glVertex3f(tempv1->x,tempv1->y, tempv1->z); 
				glVertex3f(tempv2->x,tempv2->y, tempv2->z); 
				glVertex3f(tempv3->x,tempv3->y, tempv3->z); 
				glVertex3f(tempv4->x,tempv4->y, tempv4->z); 
			glEnd();
		}
		else{
			glBegin(GL_QUADS);

				//glNormal3f(0,0,-1);
			//	glTexCoord2f(0,0);
				glTexCoord2f((atan2(tempv1->z, tempv1->x) + PI)/(2*PI), (tempv1->y + 1)/2);
				glNormal3f(tempv1->normal.x,tempv1->normal.y,tempv1->normal.z);
				glVertex3f(tempv1->x,tempv1->y, tempv1->z); 

			//	glTexCoord2f(0,1);
				glTexCoord2f((atan2(tempv2->z, tempv2->x) + PI)/(2*PI), (tempv2->y + 1)/2);
				glNormal3f(tempv2->normal.x,tempv2->normal.y,tempv2->normal.z);
				glVertex3f(tempv2->x,tempv2->y, tempv2->z); 

			//	glTexCoord2f(1,0);
				glTexCoord2f((atan2(tempv3->z, tempv3->x) + PI)/(2*PI), (tempv3->y + 1)/2);
				glNormal3f(tempv3->normal.x,tempv3->normal.y,tempv3->normal.z); 
				glVertex3f(tempv3->x,tempv3->y, tempv3->z); 

				//glTexCoord2f(1,1);
				glTexCoord2f((atan2(tempv4->z, tempv4->x) + PI)/(2*PI), (tempv4->y + 1)/2);
				glNormal3f(tempv4->normal.x,tempv4->normal.y,tempv4->normal.z); 
				glVertex3f(tempv4->x,tempv4->y, tempv4->z); 
			glEnd();
		}


	}

	

	
	glutSwapBuffers();
	glFlush();
	glDisable(GL_TEXTURE_2D);
	

	glutPostRedisplay();

}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90.0,1.0,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	float rtrx;
	float rtry;
	float rtrz;

   switch (key) {
	   //keyboard input stuff here
	   	case 'w':
			xrot += 2;
			break;
		case 's':
			xrot -= 2;
			break;
		case 'a':
			yrot += 2;
			break;
		case 'd':
			yrot -= 2;
			break;
		case 'r':
			zoom += .1;
			break;
		case 'f':
			zoom -= .1;
			break;
		case 'm':
			showMesh = !showMesh;
			break;
		case 'c':
			
			checkCC = true;
			break;
		case 'v':
			checkFlush = true;
			break;
		case 27:
			exit(0);
			break;
   }
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (1000, 1000); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init();
	checkCC = false;
	checkFlush = false;
	showMesh = false;

	cntlPt0.x = -2;
	cntlPt1.x = 0;
	cntlPt2.x = 2;
	cntlPt3.x = 0;
	cntlPt0.y = -2;
	cntlPt1.y = 3;
	cntlPt2.y = 7;
	cntlPt3.y = -4;
	cntlPt0.z = 0;
	cntlPt1.z = 4;
	cntlPt2.z = 0;
	cntlPt3.z = -4;



   //init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
  
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
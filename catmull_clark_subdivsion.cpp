#include <GLUT/GLUT.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#define PI 3.14159265  // Should be used from mathlib

using namespace std;
using namespace Eigen;

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
string USAGE = "Usage:  Uniform tessellation: /as3 InputFileName SubdivisionParameter\n        Adaptive tessellation: /as3 InputFileName SubdivisionParameter -a";
//Boolean checking if we have uniform tesstellation or not
bool uniformT;
//The allowable value of edge test in adapative tesstellation
float allowable;
//The step of the uniform tesstellation
float step;
//Boolean of the smooth mode or flat mode
bool smooth;
//Boolean of the wireframe mode
bool wireframe;
// The current x y z angles
GLfloat  xAngle = 0.0, yAngle = 0.0, zAngle = 0.0;
// The current x y z angles
GLfloat  xTrans = 0.0, yTrans = 0.0, zTrans = 0.0;
//Scale value
GLfloat scale = 1.0;
GLfloat zoom = 0.1;

//****************************************************
// Simple init function
//****************************************************
void OnInit() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

class Surface{
public:
	vector<Vector3f> controlPoints;
};

vector<Surface> surfaces;

class Vertex{
public:
	Vector3f position;
	Vector3f normal;
	float u, v;
};

class Triangle{
public:
	Vertex v1, v2, v3;
	Surface surf;
};

// given the control points of a bezier curve
// and a parametric value, return the curve
// point and derivative
Vertex bezcurveinterp(vector<Vector3f> curve, float u) {
	// first, split each of the three segments
	// to form two new ones AB and BC
	Vector3f A = curve[0] * (1.0-u) + curve[1] * u;
	Vector3f B = curve[1] * (1.0-u) + curve[2] * u;
	Vector3f C = curve[2] * (1.0-u) + curve[3] * u;
	// now, split AB and BC to form a new segment DE
	Vector3f D = A * (1.0-u) + B * u;
	Vector3f E = B * (1.0-u) + C * u;
	//finally, pick the right point on DE,
	//this is the point on the curve
	Vector3f p = D * (1.0-u) + E * u;
	// compute derivative also
	Vector3f dPdu = 3 * (E - D);
	Vertex result;
	result.position = p;
	result.normal = dPdu;
	result.normal.normalize();
	return result;
}

//given a control patch and (u,v) values, find
//the surface point and normal
Vertex bezpatchinterp(Surface sur, float u, float v) {
	//build control points for a Bezier curve in v
	vector<Vector3f> vcurve;
	vector<Vector3f> ucurve;
	for(int i = 0; i < 4; i += 1){
		vector<Vector3f> cur;
		cur.clear();
		for(int j = 0; j < 4; j += 1) {
			cur.push_back(sur.controlPoints[4 * i + j]);
		}
		Vertex cur_ver = bezcurveinterp(cur, u);
		vcurve.push_back(cur_ver.position);
	}
	//build control points for a Bezier curve in u
	for(int i = 0; i < 4; i += 1){
		vector<Vector3f> cur;
		cur.clear();
		for(int j = 0; j < 4; j += 1) {
			cur.push_back(sur.controlPoints[i + 4 * j]);
		}
		Vertex cur_ver = bezcurveinterp(cur, v);
		ucurve.push_back(cur_ver.position);
	}
	//evaluate surface and derivative for u and v
	Vertex cur_vertex1;
	Vertex cur_vertex2;
	cur_vertex1 = bezcurveinterp(vcurve, v);
	cur_vertex2 = bezcurveinterp(ucurve, u);
	Vector3f normal = cur_vertex2.normal.cross(cur_vertex1.normal);
	normal.normalize();
	Vertex result;
	result.position = cur_vertex1.position;
	result.normal = normal;
	result.u = u;
	result.v = v;
	return result;
}

// given a patch, perform uniform subdivision
vector<vector<Vertex> > subdividepatch(Surface sur, float step) {
	//compute how many subdivisions there are for this step size
	vector<vector<Vertex> > vertices;
	int numdiv = ((1 + 0.0001) / step);
	//for each parametric value of u
	for (int iu = 0; iu <= numdiv; iu += 1) {
		vector<Vertex> rows;
		rows.clear();
		float u = iu * step;
		//for each parametric value of v
		for (int iv = 0; iv <= numdiv; iv += 1) {
			float v = iv * step;
			//evaluate surface
			Vertex now;
			now = bezpatchinterp(sur, u, v);
			rows.push_back(now);
		}
		vertices.push_back(rows);
	}
	return vertices;
}
//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	// prevent a division by zero when setting aspect ratio
	if (h==0)
		h=1;

	// set the drawable region of the window
	glViewport(0,0,w,h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// just use a perspective projection
	gluPerspective(45,(float)w/h,0.1,100);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);
}

bool edgeTest(Vertex v1, Vertex v2, Surface sur) {
	Vertex v12 = bezpatchinterp(sur, 0.5 * (v1.u + v2.u), 0.5 * (v1.v + v2.v));
	Vector3f middleOfV1V2 = 0.5* (v1.position + v2.position);
	float distance = (v12.position - middleOfV1V2).norm();
	if (distance <= allowable) {
		return true;
	} else {
		return false;
	}
}
//Draw the triangles with GL_Triangle
void subdivide(Triangle t) {
	bool e1, e2, e3;
	e1 = edgeTest(t.v1, t.v2, t.surf);
	e2 = edgeTest(t.v2, t.v3, t.surf);
	e3 = edgeTest(t.v1, t.v3, t.surf);
	if (e1 && e2 && e3) {
		glNormal3f(t.v1.normal[0], t.v1.normal[1], t.v1.normal[2]);
		glVertex3f(t.v1.position[0], t.v1.position[1], t.v1.position[2]);
		glNormal3f(t.v2.normal[0], t.v2.normal[1], t.v2.normal[2]);
		glVertex3f(t.v2.position[0], t.v2.position[1], t.v2.position[2]);
		glNormal3f(t.v3.normal[0], t.v3.normal[1], t.v3.normal[2]);
		glVertex3f(t.v3.position[0], t.v3.position[1], t.v3.position[2]);
	} else if (!e1 && e2 && e3) {
		Vertex v12 = bezpatchinterp(t.surf, 0.5 * (t.v1.u + t.v2.u), 0.5 * (t.v1.v + t.v2.v));
		Triangle t1, t2;
		t1.v1 = t.v1;
		t1.v2 = v12;
		t1.v3 = t.v3;
		t1.surf = t.surf;
		t2.v1 = v12;
		t2.v2 = t.v2;
		t2.v3 = t.v3;
		t2.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
	} else if (e1 && !e2 && e3) {
		Vertex v23 = bezpatchinterp(t.surf, 0.5 * (t.v2.u + t.v3.u), 0.5 * (t.v2.v + t.v3.v));
		Triangle t1, t2;
		t1.v1 = t.v1;
		t1.v2 = t.v2;
		t1.v3 = v23;
		t1.surf = t.surf;
		t2.v1 = t.v1;
		t2.v2 = v23;
		t2.v3 = t.v3;
		t2.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
	} else if (e1 && e2 && !e3) {
		Vertex v31 = bezpatchinterp(t.surf, 0.5 * (t.v3.u + t.v1.u), 0.5 * (t.v3.v + t.v1.v));
		Triangle t1, t2;
		t1.v1 = t.v1;
		t1.v2 = t.v2;
		t1.v3 = v31;
		t1.surf = t.surf;
		t2.v1 = v31;
		t2.v2 = t.v2;
		t2.v3 = t.v3;
		t2.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
	} else if (!e1 && !e2 && e3) {
		Vertex v12 = bezpatchinterp(t.surf, 0.5 * (t.v1.u + t.v2.u), 0.5 * (t.v1.v + t.v2.v));
		Vertex v23 = bezpatchinterp(t.surf, 0.5 * (t.v2.u + t.v3.u), 0.5 * (t.v2.v + t.v3.v));
		Triangle t1, t2, t3;
		t1.v1 = t.v1;
		t1.v2 = v12;
		t1.v3 = v23;
		t1.surf = t.surf;
		t2.v1 = v12;
		t2.v2 = t.v2;
		t2.v3 = v23;
		t2.surf = t.surf;
		t3.v1 = t.v1;
		t3.v2 = v23;
		t3.v3 = t.v3;
		t3.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
		subdivide(t3);
	} else if (!e1 && e2 && !e3) {
		Vertex v12 = bezpatchinterp(t.surf, 0.5 * (t.v1.u + t.v2.u), 0.5 * (t.v1.v + t.v2.v));
		Vertex v31 = bezpatchinterp(t.surf, 0.5 * (t.v3.u + t.v1.u), 0.5 * (t.v3.v + t.v1.v));
		Triangle t1, t2, t3;
		t1.v1 = t.v1;
		t1.v2 = v12;
		t1.v3 = v31;
		t1.surf = t.surf;
		t2.v1 = v31;
		t2.v2 = v12;
		t2.v3 = t.v3;
		t2.surf = t.surf;
		t3.v1 = v12;
		t3.v2 = t.v2;
		t3.v3 = t.v3;
		t3.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
		subdivide(t3);
	} else if (e1 && !e2 && !e3) {
		Vertex v23 = bezpatchinterp(t.surf, 0.5 * (t.v2.u + t.v3.u), 0.5 * (t.v2.v + t.v3.v));
		Vertex v31 = bezpatchinterp(t.surf, 0.5 * (t.v3.u + t.v1.u), 0.5 * (t.v3.v + t.v1.v));
		Triangle t1, t2, t3;
		t1.v1 = t.v1;
		t1.v2 = t.v2;
		t1.v3 = v31;
		t1.surf = t.surf;
		t2.v1 = v31;
		t2.v2 = t.v2;
		t2.v3 = v23;
		t2.surf = t.surf;
		t3.v1 = v31;
		t3.v2 = v23;
		t3.v3 = t.v3;
		t3.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
		subdivide(t3);
	} else if (!e1 && !e2 && !e3) {
		Vertex v12 = bezpatchinterp(t.surf, 0.5 * (t.v1.u + t.v2.u), 0.5 * (t.v1.v + t.v2.v));
		Vertex v23 = bezpatchinterp(t.surf, 0.5 * (t.v2.u + t.v3.u), 0.5 * (t.v2.v + t.v3.v));
		Vertex v31 = bezpatchinterp(t.surf, 0.5 * (t.v3.u + t.v1.u), 0.5 * (t.v3.v + t.v1.v));
		Triangle t1, t2, t3, t4;
		t1.v1 = t.v1;
		t1.v2 = v12;
		t1.v3 = v31;
		t1.surf = t.surf;
		t2.v1 = v12;
		t2.v2 = t.v2;
		t2.v3 = v23;
		t2.surf = t.surf;
		t3.v1 = v31;
		t3.v2 = v23;
		t3.v3 = t.v3;
		t3.surf = t.surf;
		t4.v1 = v31;
		t4.v2 = v12;
		t4.v3 = v23;
		t4.surf = t.surf;
		subdivide(t1);
		subdivide(t2);
		subdivide(t3);
		subdivide(t4);
	}
}

//Adaptive tessellation of the given surfaces
void adaptive_tessellation(){
	//GL_lights arrangement
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
 
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i < surfaces.size(); i += 1) {
		Surface sur = surfaces[i];
		Vertex v1 = bezpatchinterp(sur, 0, 0);
		Vertex v2 = bezpatchinterp(sur, 1, 0);
		Vertex v3 = bezpatchinterp(sur, 1, 1);
		Vertex v4 = bezpatchinterp(sur, 0, 1);
		Triangle t1, t2;
		t1.v1 = v1;
		t1.v2 = v2;
		t1.v3 = v3;
		t1.surf = sur;
		t2.v1 = v1;
		t2.v2 = v3;
		t2.v3 = v4;
		t2.surf = sur;
		subdivide(t1);
		subdivide(t2);
	}
	glEnd();
}

//Show the points by using GL_Points
void uniform_tessellation_point(){
	glColor3f(1,0,1);
	glPointSize(2);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < surfaces.size(); i += 1) {
		Surface sur = surfaces[i];
		vector<vector<Vertex> > vertices = subdividepatch(sur, 0.1);
		for(size_t j = 0; j < vertices.size(); j += 1) {
			for(size_t k = 0; k < vertices[j].size(); k += 1){
				float x = vertices[j][k].position[0];
				float y = vertices[j][k].position[1];
				float z = vertices[j][k].position[2];
				glVertex3f(x,y,z);
			}
		}
	}
	glEnd();
}

// Uniform tessellation of the given surfaces
void uniform_tessellation(){

	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
 
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glBegin(GL_QUADS);
	for(size_t i = 0; i < surfaces.size(); i += 1) {
		Surface sur = surfaces[i];
		vector<vector<Vertex> > vertices = subdividepatch(sur, step);
		for(size_t j = 0; j < vertices.size() - 1; j += 1) {
			for(size_t k = 0; k < vertices.size() - 1; k += 1){
				Vertex v1 = vertices[j][k];
				Vertex v2 = vertices[j + 1][k];
				Vertex v3 = vertices[j + 1][k + 1];
				Vertex v4 = vertices[j][k + 1];
				glNormal3f(v1.normal[0],v1.normal[1],v1.normal[2]);
				glVertex3f(v1.position[0],v1.position[1],v1.position[2]);
				glNormal3f(v2.normal[0],v2.normal[1],v2.normal[2]);
				glVertex3f(v2.position[0],v2.position[1],v2.position[2]);
				glNormal3f(v3.normal[0],v3.normal[1],v3.normal[2]);
				glVertex3f(v3.position[0],v3.position[1],v3.position[2]);
				glNormal3f(v4.normal[0],v4.normal[1],v4.normal[2]);
				glVertex3f(v4.position[0],v4.position[1],v4.position[2]);
			}
		}
	}
	glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glScalef(scale, scale, scale);
	// clear the previous transform
	glLoadIdentity();

	// set the camera position
	gluLookAt(	0,10,0,	//	eye pos
				0,0,0,	//	aim point
				0,0,1);	//	up direction
    glPushMatrix ();
	glTranslatef(xTrans, yTrans, zTrans);
    glScalef (scale, scale, scale);
    glRotatef (zAngle, 0,0,1);
    glRotatef (yAngle, 0,1,0);
    glRotatef (xAngle, 1,0,0);
	if(uniformT){
		uniform_tessellation();
	} else {
		adaptive_tessellation();
	}
    glPopMatrix ();


	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
}

//****************************************************
// function that shut off the program when space is pressed
//***************************************************
void keyPressed(unsigned char key, int x, int y) {  
	if (key == ' ') {exit(0);}
	if (key == 's') {
		if (smooth) {
			glShadeModel(GL_FLAT);
			smooth = false;
		} else {
			glShadeModel(GL_SMOOTH);
			smooth = true;
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
	//zoom in
	if (key == '+') {
		scale += 0.1;
	}
	if (key == '-') {
		scale -= 0.1;
	}
	if (key == GLUT_KEY_UP) {
		xAngle += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		xAngle -= 10;
	}
	if (key == GLUT_KEY_RIGHT) {
		zAngle += 10;
	}
	if (key == GLUT_KEY_LEFT) {
		zAngle -= 10;
	}
	glutPostRedisplay();
}  

void keySpecial(int key, int x, int y) {
	bool shiftDown = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
	if (shiftDown && (key == GLUT_KEY_LEFT)) {
		xTrans += 0.1;
    } else if (shiftDown && (key == GLUT_KEY_RIGHT)) {
		xTrans -= 0.1;
    } else if (shiftDown && (key == GLUT_KEY_UP)) {
		zTrans += 0.1;
    } else if (shiftDown && (key == GLUT_KEY_DOWN)) {
		zTrans -= 0.1;
    } else if (key == GLUT_KEY_LEFT) {
		zAngle -= 10;
    } else if (key == GLUT_KEY_RIGHT) {
		zAngle += 10;
    } else if (key == GLUT_KEY_UP) {
		xAngle += 10;
	} else if (key == GLUT_KEY_DOWN) {
		xAngle -= 10;
	}
	glutPostRedisplay();
}
//****************************************************
// The main function that will run the program
//****************************************************
int main(int argc, char *argv[]) {
	//Parsing the command lines
	string file_name;
	float sub_par;
	//Vector of polygons
	if(argc > 4 || argc < 3){
		cout<<USAGE<<endl;
	} else {
		file_name = argv[1];
		//Parse the input file
		ifstream inFile(file_name);
		if (!inFile){
			cerr << "unable to open input file: "<< file_name << " --bailing out! \n";
			return 0;
		}
		const int MAXLINE=256;
		string strOneLine;
		int num_surface;
		bool firstLine = true;
		Surface cur_surface;
		while (inFile)
		{
			getline(inFile, strOneLine);
			string delimiter = " ";
			vector<string> strings;
			size_t pos = 0;
			string token;
			while ((pos = strOneLine.find(delimiter)) != string::npos) {
				token = strOneLine.substr(0, pos);
				if(token[0]){
				strings.push_back(token);
				}
				strOneLine.erase(0, pos + delimiter.length());
			}
			if(strOneLine[0]){
				strings.push_back(strOneLine);
			}
			// different input strings here, but have no clue of what input file will look like.
			if(strings.size() >= 1 && firstLine){
				num_surface = atoi(strings[0].c_str());
				firstLine = false;
			} else if(strings.size() >= 1) {
				Vector3f cur_control_point;
				float x, y, z;
				for(size_t m = 0; m < strings.size(); m += 1) {
					if(m % 3 == 0) {
						x = atof(strings[m].c_str());
					} else if (m % 3 == 1) {
						y = atof(strings[m].c_str());
					} else {
						z = atof(strings[m].c_str());
						cur_control_point = Vector3f(x, y, z);
						cur_surface.controlPoints.push_back(cur_control_point);
					}
					if(cur_surface.controlPoints.size()==16){
						surfaces.push_back(cur_surface);
						cur_surface = Surface();
					}
				}
			}
		}
		inFile.close();
		sub_par = atof(argv[2]);
		if(argc == 4) {
			string arg = argv[3];
			if(arg.compare("-a") == 0){
				uniformT = false;
				allowable = sub_par;
			} else {
				cout<<USAGE<<endl;
			}
		} else {
			uniformT = true;
			step = sub_par;
		}
	}
  //This initializes glut
  glutInit(&argc, argv);
  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
  // Initalize theviewport size
  viewport.w = 640;
  viewport.h = 480;
  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  OnInit();					// quick function to set up scene
  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(keyPressed);
  glutSpecialFunc(keySpecial);
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  return 0;
}




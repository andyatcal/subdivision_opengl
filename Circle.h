#include <Eigen/Dense>

using namespace Eigen;

class Circle{
public:
	Vector3f center;
	float radius;
	Vector3f normal;
	Circle();
	Circle(Vector3f center, float radius, Vector3f normal);
};

Circle::Circle(){
	center = Vector3f(0, 0, 0);
	radius = 0.0f;
	normal = Vector3f(0, 0, 0);
}

Circle::Circle(Vector3f center, float radius, Vector3f normal){
	this -> center = center;
	this -> radius = radius;
	this -> normal = normal;
}
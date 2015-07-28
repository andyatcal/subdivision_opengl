#include <Eigen/Dense>

using namespace Eigen;

class Circle{
public:
	Vector3f center;
	float radius;
	Circle();
	Circle(Vector3f center, float radius);
};

Circle::Circle(){
	center = Vector3f(0, 0, 0);
	radius = 0.0f;
}

Circle::Circle(Vector3f center, float radius){
	this -> center = center;
	this -> radius = radius;
}
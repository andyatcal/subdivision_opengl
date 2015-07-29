#include <glm/glm.hpp>

using namespace glm;

class Circle{
public:
	vec3 center;
	float radius;
	vec3 normal;
	Circle();
	Circle(vec3 center, float radius, vec3 normal);
};

Circle::Circle(){
	center = vec3(0, 0, 0);
	radius = 0.0f;
	normal = vec3(0, 0, 0);
}

Circle::Circle(vec3 center, float radius, vec3 normal){
	this -> center = center;
	this -> radius = radius;
	this -> normal = normal;
}
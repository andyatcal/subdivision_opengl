// This is an implementation of arcball mouse interface.
// Reference from https://github.com/subokita/Arcball/blob/master/Arcball/ (Copyright (c) 2014 Saburo Okita.)

#ifndef __Arcball__Arcball__
#define __Arcball__Arcball__

#include <iostream>
#include <Eigen/Dense>
#include <GLUT/GLUT.h>
#include <math>

class Arcball {
private:
    int windowWidth;
    int windowHeight;
    int mouseEvent;
    GLfloat rollSpeed;
    GLfloat angle ;
    Vector3f prevPos;
    Vector3f currPos;
    Vector3f camAxis;   
    bool xAxis;
    bool yAxis;

public:
    Arcball( int window_width, int window_height, GLfloat roll_speed = 1.0f, bool x_axis = true, bool y_axis = true );
    Vector3f toScreenCoord( double x, double y );

    void mouseButtonCallback( int button, int action, int mods );
    void cursorCallback( double x, double y );

    Matrix4f createViewRotationMatrix();
    Matrix4f createModelRotationMatrix( Matrix4f & view_matrix );
};
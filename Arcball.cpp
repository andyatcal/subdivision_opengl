// This is an implementation of arcball mouse interface.
// Reference from https://github.com/subokita/Arcball/blob/master/Arcball/ (Copyright (c) 2014 Saburo Okita.)

#include "Arcball.h"

/**
 * Constructor.
 * @param roll_speed the speed of rotation
 */
Arcball::Arcball( int window_width, int window_height, GLfloat roll_speed, bool x_axis, bool y_axis  ) {
    this->windowWidth  = window_width;
    this->windowHeight = window_height;
    
    this->mouseEvent = 0;
    this->rollSpeed  = roll_speed;
    this->angle      = 0.0f;
    this->camAxis    = Vector3f(0.0f, 1.0f, 0.0f);
    
    this->xAxis = x_axis;
    this->yAxis = y_axis;
}

/**
 * Convert the mouse cursor coordinate on the window (i.e. from (0,0) to (windowWidth, windowHeight))
 * into normalized screen coordinate (i.e. (-1, -1) to (1, 1)
 */
Vector3f Arcball::toScreenCoord( double x, double y ) {
    Vector3f coord(0, 0, 0);
    
    if( xAxis )
        coord[0] =  (2 * x - windowWidth ) / windowWidth;
    
    if( yAxis )
        coord[1] = -(2 * y - windowHeight) / windowHeight;
    
    /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
    coord[0] = min(max(coord[0], -1.0f), 1.0f);
    coord[1] = min(max(coord[1], -1.0f), 1.0f);
    
    float length_squared = coord[0] * coord[0] + coord[1] * coord[1];
    if( length_squared <= 1.0 )
        coord[2] = sqrt( 1.0 - length_squared );
    else
        coord = glm::normalize( coord );
    
    return coord;
}

/**
 * Check whether we should start the mouse event
 * Event 0: when no tracking occured
 * Event 1: at the start of tracking, recording the first cursor pos
 * Event 2: tracking of subsequent cursor movement
 */
void Arcball::mouseButtonCallback( GLFWwindow * window, int button, int action, int mods ){
    /*
    mouseEvent = ( action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT );
    */
}

void Arcball::cursorCallback( GLFWwindow *window, double x, double y ){
    /*
    if( mouseEvent == 0 )
        return;
    else if( mouseEvent == 1 ) {
        /* Start of trackball, remember the first position */
        prevPos     = toScreenCoord( x, y );
        mouseEvent  = 2;
        return;
    }

    
    /* Tracking the subsequent */
    currPos  = toScreenCoord( x, y );
    
    /* Calculate the angle in radians, and clamp it between 0 and 90 degrees */
    angle    = acos( std::min(1.0f, glm::dot(prevPos, currPos) ));
    
    /* Cross product to get the rotation axis, but it's still in camera coordinate */
    camAxis  = glm::cross( prevPos, currPos );
}

/**
 * Create rotation matrix within the camera coordinate,
 * multiply this matrix with view matrix to rotate the camera
 */
Matrix4f Arcball::createViewRotationMatrix() {
    return glm::rotate( glm::degrees(angle) * rollSpeed, camAxis );
}

/**
 * Create rotation matrix within the world coordinate,
 * multiply this matrix with model matrix to rotate the object
 */
Matrix4f Arcball::createModelRotationMatrix( Matrix4f & view_matrix ){
    glm::vec3 axis = glm::inverse(glm::mat3(view_matrix)) * camAxis;
    return glm::rotate( glm::degrees(angle) * rollSpeed, axis );
}
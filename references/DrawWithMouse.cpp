/*
  Program to draw Polylines using Mouse Events in OpenGL
  Author: Sarthak Kothari   Author Link: https://twitter.com/KothariSarthak
  www.pracspedia.com 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
#include <vector>
struct point
{
    int x;
    int y;
};
 
std::vector <point> points;
point OnePoint;

void lineSegment (void)
{
    glClear (GL_COLOR_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);           
    glBegin(GL_LINE_LOOP);
    for(int i =0; i < points.size();i++)
    {
        glVertex2i(points[i].x, points[i].y);
    }    
    glEnd();
    glFlush();
}
 
void processMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN)
    {      
        OnePoint.x= x;
        OnePoint.y= y; 
        points.push_back(OnePoint);
        for (int i =0; i <= points.size() -1; i++)
        {
            printf("OnePoint is %d  %d\n",points[i].x, points[i].y);
        }
        printf("*****\n");
        glutPostRedisplay();  
    }
}
 
void resize(int width, int height)
{
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION);
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);
           
}
 
int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(50,100);
    glutInitWindowSize(512,512);
    glutCreateWindow("Drawing polyline with mouse interactive");
    glutMouseFunc(processMouse);
    glutDisplayFunc(lineSegment);
    glutReshapeFunc(resize);
    glutMainLoop();
}
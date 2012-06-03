#include "TestRenderObject.h"
#include "GL/gl.h"


TestRenderObject::TestRenderObject()
{
    position = Vector(350, 300);
    position.interpolateTo(Vector(450, 300), 0.3f, -1, true, true);
    rotation.interpolateTo(360, 2, -1);
    scale = Vector(0.3f,  0.3f);
    scale.interpolateTo(Vector(1.5f, 1.5f), 1, -1, true, true);
}

TestRenderObject::~TestRenderObject()
{
}


void TestRenderObject::onRender()
{
    glColor3f(0, 0, 0.5f); // yellow

    glBegin(GL_QUADS);
    glVertex2f(400, -300);
    glVertex2f(-400, -300);
    glVertex2f(-400, 300);
    glVertex2f(400, 300);
    glEnd();



    glPointSize(40);

    glBegin(GL_POINTS);
    glColor3f(1, 1, 1); // white
    glVertex2f(-100,-100);

    glColor3f(1, 0, 0); // red
    glVertex2f(100,-100);

    glColor3f(0, 1, 0); // green
    glVertex2f(-100,100);

    glColor3f(1, 1, 0); // yellow
    glVertex2f(100,100);

    glEnd();
}
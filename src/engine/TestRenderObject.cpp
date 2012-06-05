#include "TestRenderObject.h"
#include "GL/gl.h"
#include "Engine.h"
#include "Texture.h"


TestRenderObject::TestRenderObject()
{
    position = Vector(300, 300);
    position.interpolateTo(Vector(500, 300), 0.3f, -1, true, true);
    rotation.interpolateTo(360, 1, -1);
    scale = Vector(0.5f, 0.5f);
    scale.interpolateTo(Vector(8.0f, 8.0f), 1, -1, true, true);
    loadtex = true;
}

TestRenderObject::~TestRenderObject()
{
}

void TestRenderObject::update(float dt)
{
    RenderObject::update(dt);

    if(loadtex)
    {
        setTexture("test.png");
        loadtex = false;
    }
}


void TestRenderObject::onRender() const
{
    Quad::onRender();

    /*if(tex)
    {
        tex->apply();

        float _w2 = 128;
        float _h2 = 128;
        Vector upperLeftTextureCoordinates(0, 0);
        Vector lowerRightTextureCoordinates(1, 1);

        glBegin(GL_QUADS);
        {
            glTexCoord2f(upperLeftTextureCoordinates.x, 1.0f-upperLeftTextureCoordinates.y);
            glVertex2f(-_w2, +_h2);

            glTexCoord2f(lowerRightTextureCoordinates.x, 1.0f-upperLeftTextureCoordinates.y);
            glVertex2f(+_w2, +_h2);

            glTexCoord2f(lowerRightTextureCoordinates.x, 1.0f-lowerRightTextureCoordinates.y);
            glVertex2f(+_w2, -_h2);

            glTexCoord2f(upperLeftTextureCoordinates.x, 1.0f-lowerRightTextureCoordinates.y);
            glVertex2f(-_w2, -_h2);
        }
        glEnd();
    }*/


    /*
    glColor3f(0, 0, 0.5f); // yellow

    glBegin(GL_QUADS);
    glVertex2f(400, -300);
    glVertex2f(-400, -300);
    glVertex2f(-400, 300);
    glVertex2f(400, 300);
    glEnd();
*/


    /*
    glPointSize(40);

    glBegin(GL_POINTS);
    {
        glColor3f(1, 1, 1); // white
        glVertex2f(-100,-100);

        glColor3f(1, 0, 0); // red
        glVertex2f(100,-100);

        glColor3f(0, 1, 0); // green
        glVertex2f(-100,100);

        glColor3f(1, 1, 0); // yellow
        glVertex2f(100,100);
    }
    glEnd();
    */
}
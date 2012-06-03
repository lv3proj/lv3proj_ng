#include "RenderObject.h"
#include "GL/gl.h"

const bool DEBUG_RENDER = true;


RenderObject::RenderObject()
{
    scale = Vector(1, 1, 1);
    color = Vector(1, 1, 1);
    alpha = 1;
    alpha2 = 1;
}

RenderObject::~RenderObject()
{
}

void RenderObject::update(float dt)
{
    position.update(dt);
    offset.update(dt);
    scale.update(dt);
    color.update(dt);
    alpha.update(dt);
    alpha2.update(dt);
    rotation.update(dt);
    rotation2.update(dt);
    velocity.update(dt);
    gravity.update(dt);

    onUpdate(dt);
}

void RenderObject::render()
{
    Vector renderPos = position + offset;
    Vector renderRot = rotation + rotation2;
    float renderAlpha = alpha.x + alpha2.x;

    glPushMatrix();
    glTranslatef(renderPos.x, renderPos.y, renderPos.z);

    glRotatef(renderRot.x, 0, 0, 1); 
    /*if (isfh())
    {
    glDisable(GL_CULL_FACE);
    glRotatef(180, 0, 1, 0);
    }*/

    glScalef(scale.x, scale.y, 1);

    glColor4f(color.x, color.y, color.z, renderAlpha);


    // TODO

    onRender();


    if(DEBUG_RENDER)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glLineWidth(4);
        glEnable(GL_BLEND);

        int i = 0;
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex2f(renderPos.x, renderPos.y);
    }

    // TODO

    glPopMatrix();
}


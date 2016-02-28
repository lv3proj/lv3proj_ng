#include "Entity.h"
#include "collision/AABB.h"
#include "collision/Circle.h"
#include "collision/Line.h"
#include "Engine.h"
#include "Renderer.h"


Entity::Entity()
 : _collider(NULL)
{
}

Entity::~Entity()
{
    delete _collider;
}

void Entity::destroy()
{
    delete this;
}

void Entity::update(float dt)
{
    Quad::update(dt);

}

void Entity::setCollider(Collidable *c)
{
    if(_collider)
        delete _collider;

    _collider = c;
}

void Entity::onRender() const
{
    Quad::onRender();
}

void Entity::onUpdate(float dt)
{
    // Intentionally NOT calling RenderObject::onUpdate().
}

void Entity::updatePhysics(float dt)
{
    Vector lastPos = position;

    Quad::updatePhysics(dt);

    /*Vector cp;
    if(engine->obsgrid.collidesWith(_collider, &cp))
    {
        position = lastPos;
        Vector wn;
        if(engine->obsgrid.getNormal(cp, wn))
        {
            Vector t = speed.getReflection2D(wn);
            speed = t;
        }
        else
            logerror("WTF");
    }*/

}


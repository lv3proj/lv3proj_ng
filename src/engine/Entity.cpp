#include "Entity.h"
#include "Collision/AABB.h"
#include "Collision/Circle.h"
#include "Collision/Line.h"
#include "Engine.h"
#include "Renderer.h"

bool Entity::s_renderCollisionShapes = false;


Entity::Entity()
 : _collider(NULL)
{
}

Entity::~Entity()
{
    delete _collider;
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

    // DEBUG
    if(s_renderCollisionShapes && _collider)
    {
        switch(_collider->getShape())
        {
            case COLL_AABB:
            {
                // FIXME: this messes up if we're part of a RO hierarchy
                // HACK: undo gfx rotation
                const AABB& aabb = *(const AABB*)_collider;
                Vector ul = aabb.upleft();
                Vector dr = aabb.downright();
                engine->GetRenderer()->drawAABB(ul.x - position.x,
                                                ul.y - position.y,
                                                dr.x - position.x,
                                                dr.y - position.y,
                                                rotation.x + rotation2.x, 0, 1, 0, 0.5f);
                break;
            }

            case COLL_CIRCLE:
            {
                const Circle& ci = *(const Circle*)_collider;
                Vector cpos = ci.getPosition();
                engine->GetRenderer()->drawCircle(cpos.x, cpos.y, ci.radius, 0, 1, 0, 0.5f);
                break;
            }

            case COLL_LINE:
            {
                const Line& l = *(const Line*)_collider;
                Vector start = l.start();
                Vector end = l.end();
                // HACK: undo gfx rotation
                const float rot = rotation.x + rotation2.x;
                start.rotate2D(-rot);
                end.rotate2D(-rot);
                engine->GetRenderer()->drawLine(start.x, start.y, end.x, end.y, 2, 0, 1, 0, 0.5f);
                break;
            }
        }
    }

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


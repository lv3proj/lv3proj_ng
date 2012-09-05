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

    // FIXME: this might get a problem with final rotation if we're at the end
    // of a bone chain or something
    if(_collider)
        _collider->updatePosition(position + offset, rotation + rotation2);
}

void Entity::setCollider(Collidable *c)
{
    if(_collider)
        delete _collider;

    c->updatePosition(position + offset, rotation + rotation2);
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
                engine->GetRenderer()->drawAABB(aabb.x1() - position.x,
                                                aabb.y1() - position.y,
                                                aabb.x2() - position.x,
                                                aabb.y2() - position.y,
                                                rotation.x + rotation2.x, 0, 1, 0, 0.5f);
                break;
            }

            case COLL_CIRCLE:
            {
                const Circle& ci = *(const Circle*)_collider;
                engine->GetRenderer()->drawCircle(ci.position.x, ci.position.y, ci.radius, 0, 1, 0, 0.5f);
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

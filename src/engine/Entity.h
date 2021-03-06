#ifndef ENTITY_H
#define ENTITY_H

#include "Quad.h"
#include "collision/Collidable.h"

class Entity : public Quad
{
public:

    virtual void update(float dt);
    virtual void onRender() const;
    virtual void updatePhysics(float dt);

    virtual void destroy();

    inline bool collidesWith(const Entity *other, Vector *result);
    inline Collidable *getCollider() const { return _collider; }
    void setCollider(Collidable *c);

    float bounciness;

    static bool s_renderCollisionShapes;

protected:

    Entity();
    virtual ~Entity();

    virtual void onUpdate(float dt);

    Collidable *_collider;

};


bool Entity::collidesWith(const Entity *other, Vector *result)
{
    return _collider && _collider->collidesWith(other->_collider, result);
}


#endif

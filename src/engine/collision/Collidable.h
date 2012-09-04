#ifndef COLLIDABLE_H
#define COLLIDABLE_H

class AABB;

#include "Vector.h"
#include "Collision.h"

class Collidable
{
public:
    virtual ~Collidable() {}

    virtual AABB getAABB() const = 0;
    virtual void updatePosition(const Vector& pos, const Vector& rot) = 0;
    
    inline bool collidesWith(const Collidable *other, Vector *point = 0) const
    {
        return other && Collision::Check(*this, *other, point);
    }

    inline CollisionShape getShape() const { return _shape; }

    unsigned char collisionBits;

protected:
    Collidable(CollisionShape s) : _shape(s), collisionBits(~0) {}
    Collidable(const Collidable& other) : _shape(other._shape), collisionBits(~0) {}

private:
    CollisionShape _shape;
};


#endif
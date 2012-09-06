#ifndef COLLIDABLE_H
#define COLLIDABLE_H

class AABB;
class RenderObject;

#include "Vector.h"
#include "Collision.h"
#include "../RenderObject.h"


class Collidable
{
public:
    virtual ~Collidable() {}

    virtual AABB getAABB() const = 0;
    
    inline bool collidesWith(const Collidable *other, Vector *point = 0) const
    {
        return other && Collision::Check(*this, *other, point);
    }

    inline CollisionShape getShape() const { return _shape; }

    inline Vector getPosition() const
    {
        return position + offset;
    }

    inline Vector getRotation() const
    {
        return rotation + rotation2;
    }

    unsigned char collisionBits;


protected:

    Collidable(CollisionShape s, const RenderObject *ro)
        : _shape(s)
        , collisionBits(~0)
        , ro(ro)
        , position(ro->position)
        , offset(ro->offset)
        , rotation(ro->rotation)
        , rotation2(ro->rotation2)
    {
    }

    Collidable(const Collidable& other)
        : _shape(other._shape)
        , collisionBits(other.collisionBits)
        , ro(other.ro)
        , position(other.position)
        , offset(other.offset)
        , rotation(other.rotation)
        , rotation2(other.rotation2)
    {
    }

    Collidable(CollisionShape s, const Vector& pos = Vector())
        : _shape(s)
        , collisionBits(~0)
        , ro(NULL)
        , _position(pos) // Set internal position and reference that, instead of external pos
        , position(_position)
        , offset(nullv)
        , rotation(nullv)
        , rotation2(nullv)
    {
    }

    const RenderObject *ro;

    static const Vector nullv;

private:
    CollisionShape _shape;
    Vector _position;

    const Vector &position;
    const Vector &offset;
    const Vector &rotation;
    const Vector &rotation2;
    //const Vector &scale;
};


#endif
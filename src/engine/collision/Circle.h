#ifndef SHAPE_CIRCLE_H
#define SHAPE_CIRCLE_H

#include "Collidable.h"

class AABB;

class Circle : public Collidable
{
public:
    Circle()
        : Collidable(COLL_CIRCLE)
        , radius(0)
    {
    }

    Circle(const Circle& c)
        : Collidable(c)
        , radius(c.radius)
        , position(c.position)
    {
    }

    Circle(const Vector& pos, float r)
        : Collidable(COLL_CIRCLE)
        , position(pos)
        , radius(r)
    {
    }

    virtual AABB getAABB() const;


    Vector position;
    float radius;
};

#endif

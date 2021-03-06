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
    {
    }

    Circle(const RenderObject *ro, float r)
        : Collidable(COLL_CIRCLE, ro)
        , radius(r)
    {
    }

    Circle(const Vector& pos, float r)
        : Collidable(COLL_CIRCLE, pos)
        , radius(r)
    {
    }

    virtual AABB getAABB() const;

    inline bool isPointInside(const Vector& point) const
    {
        return (getPosition() - point).isLength2DIn(radius);
    }


    float radius;
};

#endif

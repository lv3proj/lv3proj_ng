#ifndef SHAPE_AABB_H
#define SHAPE_AABB_H

#include "Collidable.h"


class AABB : public Collidable
{
public:

    AABB() : Collidable(COLL_AABB) {}
    AABB(const Vector& c1, const Vector& c2)
        : Collidable(COLL_AABB)
        , upleft(c1), downright(c2)
    {
    }

    AABB(float x1, float y1, float x2, float y2)
        : Collidable(COLL_AABB)
        , upleft(x1, y1), downright(x2, y2)
    {
    }

    AABB(const AABB& other)
        : Collidable(other)
        , upleft(other.upleft), downright(other.downright)
    {
    }

    virtual AABB getAABB() const { return *this; }

    AABB getUnion(const AABB& other) const;
    AABB getOverlap(const AABB& other) const;
    inline Vector getCenter() const { return upleft + (0.5f * (downright - upleft)); }


    Vector upleft;
    Vector downright;
};

#endif

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
    virtual void updatePosition(const Vector& pos, const Vector& rot);

    AABB getUnion(const AABB& other) const;
    AABB getOverlap(const AABB& other) const;
    inline Vector getCenter() const { return upleft + (0.5f * (downright - upleft)); }

    inline void setCenter(const Vector& c)
    {
        Vector half = (downright - upleft) * 0.5f;
        upleft = c - half;
        downright = c + half;
    }

    inline float x1() const { return upleft.x; }
    inline float y1() const { return upleft.y; }
    inline float x2() const { return downright.x; }
    inline float y2() const { return downright.y; }

    Vector upleft;
    Vector downright;
};

#endif

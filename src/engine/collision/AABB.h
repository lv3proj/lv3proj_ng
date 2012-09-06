#ifndef SHAPE_AABB_H
#define SHAPE_AABB_H

#include "Collidable.h"


class AABB : public Collidable
{
public:

    AABB()
        : Collidable(COLL_AABB)
    {
    }

    AABB(const RenderObject *ro, const Vector& c1, const Vector& c2)
        : Collidable(COLL_AABB, ro)
        , upleftrel(c1), downrightrel(c2)
    {
        _fix();
    }

    AABB(const Vector& pos, const Vector& c1, const Vector& c2)
        : Collidable(COLL_AABB, pos)
        , upleftrel(c1), downrightrel(c2)
    {
        _fix();
    }

    AABB(const Vector& c1, const Vector& c2)
        : Collidable(COLL_AABB)
        , upleftrel(c1), downrightrel(c2)
    {
        _fix();
    }

    AABB(float x1, float y1, float x2, float y2)
        : Collidable(COLL_AABB)
        , upleftrel(x1, y1), downrightrel(x2, y2)
    {
        _fix();
    }

    AABB(const AABB& other)
        : Collidable(other)
        , upleftrel(other.upleftrel), downrightrel(other.downrightrel)
    {
    }

    virtual AABB getAABB() const { return *this; }

    AABB getUnion(const AABB& other) const;
    AABB getOverlap(const AABB& other) const;

    inline Vector getCenter() const { Vector ul = upleft(); return ul + (0.5f * (downright() - ul)); }

    inline bool isPointInside(const Vector& point) const
    {
        Vector p = getPosition();
        Vector ul = upleft();
        Vector dr = downright();
        
        return point.x >= ul.x && point.x <= dr.x
            && point.y >= ul.y && point.y <= dr.y;
    }

    inline Vector upleft() const { return getPosition() + upleftrel; }
    inline Vector downright() const { return getPosition() + downrightrel; }

    inline Vector upleftRel() const { return upleftrel; }
    inline Vector downrightRel() const { return downrightrel; }

    inline void _fix()
    {
        if(upleftrel.x > downrightrel.x)
            std::swap(upleftrel.x, downrightrel.x);
        if(upleftrel.y > downrightrel.y)
            std::swap(upleftrel.y, downrightrel.y);
    }

protected:

    Vector upleftrel;
    Vector downrightrel;
};

#endif

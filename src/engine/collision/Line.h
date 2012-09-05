#ifndef SHAPE_LINE_H
#define SHAPE_LINE_H

#include "Collidable.h"
#include "AABB.h"

class Line : public Collidable
{
public:

    Line() : Collidable(COLL_LINE) {}

    Line(const Line& other)
        : Collidable(other)
        , relstart(other.relstart)
        , relend(other.relend)
        , position(other.position)
        , rotation(other.rotation)
    {
    }

    Line(const Vector& start, const Vector& end)
        : Collidable(COLL_LINE)
        , relstart(start)
        , relend(end)
        , rotation(0)
    {
    }

    Line(const Vector& position, const Vector& start, const Vector& end, float rot = 0)
        : Collidable(COLL_LINE)
        , relstart(start)
        , relend(end)
        , position(position)
        , rotation(rot)
    {
    }

    virtual AABB getAABB() const { return AABB(startpos(), endpos()); }
    virtual void updatePosition(const Vector& pos, const Vector& rot);

    Vector position; // origin, point that is rotated around
    float rotation;

    inline Vector startpos() const { return position + relstart; }
    inline Vector endpos() const { return position + relend; }
    inline Vector direction() const { return relend - relstart; }
    inline const Vector& start() const { return relstart; }
    inline const Vector& end() const { return relend; }

protected:

    // relative to position
    Vector relstart;
    Vector relend;
};

#endif

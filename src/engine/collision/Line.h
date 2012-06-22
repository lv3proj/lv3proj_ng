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
        , start(other.start)
        , end(other.end)
    {
    }

    Line(const Vector& start, const Vector& end)
        : Collidable(COLL_LINE)
        , start(start)
        , end(end)
    {
    }

    virtual AABB getAABB() const { return AABB(start, end); }

    Vector start;
    Vector end;
};

#endif

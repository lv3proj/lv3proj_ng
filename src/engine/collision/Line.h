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

    template <typename T> bool tracei(T callback, Vector *v = NULL, int step = 1) const;

protected:

    // relative to position
    Vector relstart;
    Vector relend;
};


// bresenham algorithm
template<typename T> bool Line::tracei(T callback, Vector *v /* = NULL */, int step /* = 1 */) const
{
    Vector start = startpos();
    Vector end = endpos();
    int x0 = (int)start.x;
    int y0 = (int)start.y;
    int x1 = (int)end.x;
    int y1 = (int)end.y;
    int dx = abs(x1-x0);
    int dy = abs(y1-y0);
    int sx = x0 < x1 ? step : -step;
    int sy = y0 < y1 ? step : -step;
    int err = dx - dy;
    int err2;
    int lastx = x0, lasty = y0;

    while(true)
    {
        if(callback(x0, y0))
        {
            if(v)
                *v = Vector(lastx, lasty);
            return true;
        }
        lastx = x0;
        lasty = y0;
        if(x0 == x1 && y0 == y1)
            break;

        err2 = err * 2;
        if(err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if(err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }

    return false;
}

#endif

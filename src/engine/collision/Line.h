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
    {
    }

    Line(const Vector& start, const Vector& end)
        : Collidable(COLL_LINE)
        , relstart(start)
        , relend(end)
    {
    }

    Line(const RenderObject *ro, const Vector& start, const Vector& end)
        : Collidable(COLL_LINE, ro)
        , relstart(start)
        , relend(end)
    {
    }

    Line(const Vector& position, const Vector& start, const Vector& end)
        : Collidable(COLL_LINE, position)
        , relstart(start)
        , relend(end)
    {
    }

    virtual AABB getAABB() const { return AABB(ro, start(), end()); }

    inline Vector startpos() const { return getPosition() + start(); }
    inline Vector endpos() const { return getPosition() + end(); }
    inline Vector direction() const { Vector v(relend - relstart); v.rotate2D(getRotation().x); return v; }
    inline Vector start() const { Vector v(relstart); v.rotate2D(getRotation().x); return v; }
    inline Vector end() const { Vector v(relend); v.rotate2D(getRotation().x); return v;  }

    inline float len() const { return (relend - relstart).getLength2D(); }
    inline float lenSq() const { return (relend - relstart).getLength2DSq(); }

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

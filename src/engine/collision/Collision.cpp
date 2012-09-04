#include "Collision.h"
#include "Collidable.h"
#include "AABB.h"
#include "Circle.h"
#include "PixelMap.h"
#include "Line.h"
#include <assert.h>


bool Collision::Check(const Collidable& a, const Collidable& b, Vector *v)
{
    if((a.collisionBits & b.collisionBits) == 0)
        return false;

    switch(a.getShape())
    {
        case COLL_AABB:
            switch(b.getShape())
            {
                case COLL_AABB:     return AABB_vs_AABB((const AABB&)a, (const AABB&)b, v);
                case COLL_CIRCLE:   return AABB_vs_Circle((const AABB&)a, (const Circle&)b, v);
                case COLL_LINE:     return AABB_vs_Line((const AABB&)a, (const Line&)b, v);
            }
            break;

        case COLL_CIRCLE:
            switch(b.getShape())
            {
                case COLL_AABB:     return AABB_vs_Circle((const AABB&)b, (const Circle&)a, v);
                case COLL_CIRCLE:   return Circle_vs_Circle((const Circle&)a, (const Circle&)b, v);
                case COLL_LINE:     return Circle_vs_Line((const Circle&)a, (const Line&)b, v);
            }
            break;

        case COLL_LINE:
            switch(b.getShape())
            {
                case COLL_AABB:     return AABB_vs_Line((const AABB&)b, (const Line&)a, v);
                case COLL_CIRCLE:   return Circle_vs_Line((const Circle&)b, (const Line&)a, v);
                case COLL_LINE:     return Line_vs_Line((const Line&)a, (const Line&)b, v);
            }
            break;
    }

    assert(false);
    return false;
}

bool Collision::AABB_vs_AABB(const AABB& a, const AABB& b, Vector *v)
{
    bool c =  !(a.downright.y < b.upleft.y
             || a.upleft.y    > b.downright.y
             || a.downright.x < b.upleft.x
             || a.upleft.x    > b.downright.x);

    if(c && v)
    {
        AABB ov = a.getOverlap(b);
        *v = ov.getCenter();
    }

    return c;
}

bool Collision::AABB_vs_Circle(const AABB& a, const Circle& c, Vector *v)
{
    bool inx = c.position.x >= a.upleft.x
            && c.position.x <= a.downright.x;

    bool iny = c.position.y >= a.upleft.y
            && c.position.y <= a.downright.y;
    /*
      |       |    
      |  (x)  |    
  ----+-------+----
      |#######|    
   (y)|#######|(y) 
      |#######|    
  ----+-------+----
      |  (x)  |    
      |       |    
    */
    // First check: Center completely in AABB? [(#) region ]
    if(inx && iny)
    {
        if(v)
            *v = c.position;
        return true;
    }

    // Quick check: If AABBs are not intersecting, the circle is definitely out of reach.
    AABB caabb = c.getAABB();
    if(!AABB_vs_AABB(a, caabb, NULL))
        return false;

    // AABBs intersecting. Now, if the circle center is contained in at least one axis,
    // they are intersecting. [ (x) or (y) region ]
    if(inx || iny)
    {
        if(v)
        {
            *v = a.getOverlap(caabb).getCenter(); // FIXME: this is just an estimation, and probably off
        }
        return true;
    }

    // If we are here, the circle center must be in one of the corner regions.
    // Now we need to check if the circle contains the corresponding corner point of the AABB.
    Vector corner;
    // Above AABB?
    if(c.position.y <= a.upleft.y)
    {
        // Left of AABB?
        if(c.position.x <= a.upleft.y)
        {
            // Upper left
            corner = a.upleft;
        }
        else
        {
            // Upper right
            corner = Vector(a.downright.x, a.upleft.y);
        }
    }
    else
    {
        if(c.position.x <= a.upleft.y)
        {
            // Lower left
            corner = Vector(a.upleft.x, a.downright.y);
        }
        else
        {
            // Lower right.
            corner = a.downright;
        }
    }

    if(c.isPointInside(corner))
    {
        if(v)
            *v = corner; // FIXME: this is off too
        return true;
    }

    // Circle is too far away, done here.
    return false;
}

bool Collision::AABB_vs_Line(const AABB& a, const Line& b, Vector *v)
{
    // Fast AABB check first
    if(!AABB_vs_AABB(a, b.getAABB(), NULL))
        return false;

    /*       (u)
          +-------+
          |       |
       (l)|       |(r)
          |       |
          +-------+
             (d)
    */
    Vector upright(a.downright.x, a.upleft.y);
    Vector downleft(a.upleft.x, a.downright.y);

    // (u)
    if(Line_vs_Line(b, Line(a.upleft, upright), v))
        return true;

    // (l)
    if(Line_vs_Line(b, Line(a.upleft, downleft), v))
        return true;

    // (r)
    if(Line_vs_Line(b, Line(upright, a.downright), v))
        return true;

    // (d)
    if(Line_vs_Line(b, Line(downleft, a.downright), v))
        return true;

    return false;
}

bool Collision::Circle_vs_Circle(const Circle& a, const Circle& b, Vector *v)
{
    Vector diff = a.position - b.position;
    bool c = diff.isLength2DIn(a.radius + b.radius);

    if(c && v)
    {
        *v = a.position + (diff * 0.5f);
    }

    return c;
}

bool Collision::Circle_vs_Line(const Circle& a, const Line& b, Vector *v)
{
    const Vector dir = b.direction();
    const Vector bstart = b.startpos();
    Vector diff = a.position - bstart;
    Vector closest;
    if (!dir.isZero())
    {
        float t = diff.dot2D(dir) / dir.getLength2DSq();
        t = clamp(t, 0.0f, 1.0f);
        closest = bstart + (t * dir);
    }
    else
        closest = bstart;

    diff = a.position - closest;
    bool c = diff.isLength2DIn(a.radius);
    if (v && c)
        *v = closest;
    return c;
}

// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
// Based on implementation by Damian Coventry.
bool Collision::Line_vs_Line(const Line& a, const Line& b, Vector *v)
{
    const Vector astart = a.startpos();
    const Vector aend = a.endpos();
    const Vector bstart = b.startpos();
    const Vector bend = b.endpos();

    const float denom = ((bend.y - bstart.y)*(aend.x - astart.x)) -
        ((bend.x - bstart.x)*(aend.y - astart.y));

    const float nume_a = ((bend.x - bstart.x)*(astart.y - bstart.y)) -
        ((bend.y - bstart.y)*(astart.x - bstart.x));

    const float nume_b = ((aend.x - astart.x)*(astart.y - bstart.y)) -
        ((aend.y - astart.y)*(astart.x - bstart.x));

    if(denom == 0.0f)
    {
        if(nume_a == 0.0f && nume_b == 0.0f)
        {
            if(v)
                *v = astart + (aend * 0.5);
            return true; // coincident
        }
        return false; // parallel
    }

    const float ua = nume_a / denom;
    const float ub = nume_b / denom;

    if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        if(v)
        {
            *v = astart + ua*(aend - astart);
        }

        return true; // intersecting
    }

    return false;
}


#include "Collision.h"
#include "Collidable.h"
#include "AABB.h"
#include "Circle.h"
#include "PixelMap.h"
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
                case COLL_PIXMAP:   return AABB_vs_Pixmap((const AABB&)a, (const PixMap&)b, v);
            }
            break;

        case COLL_CIRCLE:
            switch(b.getShape())
            {
                case COLL_AABB:     return AABB_vs_Circle((const AABB&)b, (const Circle&)a, v);
                case COLL_CIRCLE:   return Circle_vs_Circle((const Circle&)a, (const Circle&)b, v);
                case COLL_PIXMAP:   return Circle_vs_Pixmap((const Circle&)a, (const PixMap&)b, v);
            }
            break;

        case COLL_PIXMAP:
            switch(b.getShape())
            {
                case COLL_AABB:     return AABB_vs_Pixmap((const AABB&)b, (const PixMap&)a, v);
                case COLL_CIRCLE:   return Circle_vs_Pixmap((const Circle&)b, (const PixMap&)a, v);
                case COLL_PIXMAP:   return Pixmap_vs_Pixmap((const PixMap&)a, (const PixMap&)b, v);
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
            v = a.getOverlap(caabb).getCenter(); // FIXME: this is just an estimation, and probably off
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

bool Collision::AABB_vs_Pixmap(const AABB& a, const PixMap& b, Vector *v)
{
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

bool Collision::Circle_vs_Pixmap(const Circle& a, const PixMap& b, Vector *v)
{
    return false;
}

bool Collision::Pixmap_vs_Pixmap(const PixMap& a, const PixMap& b, Vector *v)
{
    return false;
}


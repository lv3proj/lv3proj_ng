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

bool Collision::AABB_vs_Circle(const AABB& a, const Circle& b, Vector *v)
{
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


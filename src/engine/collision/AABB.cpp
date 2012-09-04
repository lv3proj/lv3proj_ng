#include "AABB.h"

void AABB::updatePosition(const Vector& pos, const Vector& /*rot*/)
{
    setCenter(pos);
}

// original code from SDL
AABB AABB::getOverlap(const AABB& o) const
{
    float Amin, Amax, Bmin, Bmax;
    AABB result;

    // X Axis
    Amin = upleft.x;
    Amax = downright.x;
    Bmin = o.upleft.x;
    Bmax = o.downright.x;
    if (Bmin > Amin)
        Amin = Bmin;
    result.upleft.x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.downright.x = Amax;

    // Y Axis
    Amin = upleft.y;
    Amax = downright.y;
    Bmin = o.upleft.y;
    Bmax = o.downright.y;
    if (Bmin > Amin)
        Amin = Bmin;
    result.upleft.y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.downright.y = Amax;

    return result;
}

// original code from SDL
AABB AABB::getUnion(const AABB& o) const
{
    float Amin, Amax, Bmin, Bmax;
    AABB uni;

    // X axis
    Amin = upleft.x;
    Amax = downright.x;
    Bmin = o.upleft.x;
    Bmax = o.downright.x;
    if (Bmin < Amin)
        Amin = Bmin;
    uni.upleft.x = Amin;
    if (Bmax > Amax)
        Amax = Bmax;
    uni.downright.x = Amax;

    // Y axis
    Amin = upleft.y;
    Amax = downright.y;
    Bmin = o.upleft.y;
    Bmax = o.downright.y;
    if (Bmin < Amin)
        Amin = Bmin;
    uni.upleft.y = Amin;
    if (Bmax > Amax)
        Amax = Bmax;
    uni.downright.y = Amax;

    return uni;
}

#include "AABB.h"

// original code from SDL
AABB AABB::getOverlap(const AABB& o) const
{
    Vector ul = upleft();
    Vector dr = downright();
    Vector ul2 = o.upleft();
    Vector dr2 = o.downright();

    float Amin, Amax, Bmin, Bmax;
    AABB result;

    // X Axis
    Amin = ul.x;
    Amax = dr.x;
    Bmin = ul2.x;
    Bmax = dr2.x;
    if (Bmin > Amin)
        Amin = Bmin;
    result.upleftrel.x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.downrightrel.x = Amax;

    // Y Axis
    Amin = ul.y;
    Amax = dr.y;
    Bmin = ul2.y;
    Bmax = dr2.y;
    if (Bmin > Amin)
        Amin = Bmin;
    result.upleftrel.y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.downrightrel.y = Amax;

    return result;
}

// original code from SDL
AABB AABB::getUnion(const AABB& o) const
{
    Vector ul = upleft();
    Vector dr = downright();
    Vector ul2 = o.upleft();
    Vector dr2 = o.downright();

    float Amin, Amax, Bmin, Bmax;
    AABB uni;

    // X axis
    Amin = ul.x;
    Amax = dr.x;
    Bmin = ul2.x;
    Bmax = dr2.x;
    if (Bmin < Amin)
        Amin = Bmin;
    uni.upleftrel.x = Amin;
    if (Bmax > Amax)
        Amax = Bmax;
    uni.downrightrel.x = Amax;

    // Y axis
    Amin = ul.y;
    Amax = dr.y;
    Bmin = ul2.y;
    Bmax = dr2.y;
    if (Bmin < Amin)
        Amin = Bmin;
    uni.upleftrel.y = Amin;
    if (Bmax > Amax)
        Amax = Bmax;
    uni.downrightrel.y = Amax;

    return uni;
}

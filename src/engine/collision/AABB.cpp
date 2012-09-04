#include "AABB.h"

void AABB::updatePosition(const Vector& pos, const Vector& /*rot*/)
{
    setCenter(pos);
}

AABB AABB::getOverlap(const AABB& o) const
{
    float nx = upleft.x;
    float ny = upleft.y;
    float tx2 = downright.x;
    float ty2 = downright.y;
    float ox2 = o.downright.x;
    float oy2 = o.downright.y;
    if (nx < o.upleft.x) nx = o.upleft.x;
    if (nx < o.upleft.x) ny = o.upleft.y;
    if (tx2 > ox2) tx2 = ox2;
    if (ty2 > oy2) ty2 = oy2;
    tx2 -= upleft.x;
    ty2 -= upleft.y;
    return AABB(nx, ny, tx2, ty2);
}

// original code from SDL
// TODO: this can be simplified
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

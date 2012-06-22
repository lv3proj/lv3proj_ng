#include "PixelMap.h"
#include "AABB.h"

AABB PixMap::getAABB() const
{
    return AABB(position, position + Vector(pix.size1d(), pix.size1d()));
}

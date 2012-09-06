#include "Circle.h"
#include "AABB.h"

AABB Circle::getAABB() const
{
    return AABB(ro, Vector(-radius, radius), Vector(-radius, radius));
}

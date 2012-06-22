#include "Circle.h"
#include "AABB.h"

AABB Circle::getAABB() const
{
    return AABB(position.x - radius, position.y - radius,
                position.x + radius, position.y + radius);
}

#include "Line.h"

void Line::updatePosition(const Vector& pos, const Vector& rot)
{
    position = pos;

    float d = rot.x - rotation;
    // FIXME: this may cause desync due to rounding errors.
    // Better to store original start/end and always rotate absolutely?
    relstart.rotate2D(d);
    relend.rotate2D(d);
    rotation = rot.x;
}


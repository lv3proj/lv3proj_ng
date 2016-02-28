#ifndef MOUSESTATE_H
#define MOUSESTATE_H

#include "Vector.h"

struct MouseState
{
    MouseState() : active(true), buttons(0), wheelAbs(0), wheelRel(0) {}
    bool active;
    Vector worldPos;
    Vector worldRel;
    Vector winPos;
    Vector winRel;
    unsigned int buttons;
    int wheelAbs;
    int wheelRel;

    bool isLeft() const;
    bool isRight() const;
    bool isMiddle() const;
};


#endif

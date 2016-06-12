#ifndef MOUSESTATE_H
#define MOUSESTATE_H

#include "glmx.h"

struct MouseState
{
    MouseState() : active(true), buttons(0), wheelAbs(0), wheelRel(0) {}
    bool active;
    glm::vec2 worldPos;
    glm::vec2 worldRel;
    glm::vec2 winPos;
    glm::vec2 winRel;
    unsigned buttons;
    int wheelAbs;
    int wheelRel;

    bool isLeft() const;
    bool isRight() const;
    bool isMiddle() const;
};


#endif

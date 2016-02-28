#include "FrameTime.h"
#include "SDLMaster.h"

FrameTime::FrameTime()
: _t(0)
{
}

void FrameTime::start()
{
    _t = SDL_GetTicks();
}

void FrameTime::getDiffMS() const
{
    unsigned t = SDL_GetTicks();
    return = t - _t;
}

float FrameTime::getDT() const
{
    unsigned ms = getDiffMS();
    return ms / 1000.0f;
}


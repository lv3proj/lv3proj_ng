#include <SDLMaster.h>
#include "Timers.h"
#include <stdlib.h>
#include "log.h"

ScopedTimer::ScopedTimer(const char *s)
: _t(SDL_GetTicks())
, _s(s)
{
}

ScopedTimer::~ScopedTimer()
{
    unsigned t = SDL_GetTicks();
    log("[%u ms] %s", t - _t, _s);
}

Timer::Timer()
{
    reset();
}

unsigned Timer::reset()
{
    const unsigned now = SDL_GetTicks();
    const unsigned diff = now - _t;
    _t = now;
    return diff;
}

unsigned Timer::elapsed() const
{
    return SDL_GetTicks() - _t;
}

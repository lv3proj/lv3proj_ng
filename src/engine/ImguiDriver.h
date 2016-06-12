#pragma once

#include "event.h"

struct SDL_Window;

class ImguiDriver : public RawEventRecv
{
public:
    ImguiDriver(SDL_Window *w);
    ~ImguiDriver();

    bool init();
    void beginFrame();
    void endFrameAndRender();

    // Inherited from RawEventRecv
    virtual void rawEvent(void*);

private:
    SDL_Window * const _win;
    bool _inited;
};


#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vector.h"

struct SDL_Surface;

struct RenderSettings
{

    float virtualW;
    float virtualH;
    int pixelsW;
    int pixelsH;
    int bpp;
    bool vsync;
    bool full;
};

// this class might serve as an interface for a software or whatever other non-OpenGL renderer later.

class Renderer
{
public:

    Renderer();
    virtual ~Renderer();

    bool Init();
    void Shutdown();

    void SetInternalResolution(float x, float y);
    void SetScreenResolution(uint32 x, uint32 y);
    void SetVsync(bool on);
    void SetBPP(int bpp);
    void SetFullscreen(bool on);
    bool ApplySettings();
    // TODO: fullscreen
    // TODO: resize support

    void clear();
    void show();
    void setupRenderPositionAndScale();

protected:

    void _Enable2D(uint32 x, uint32 y);
    SDL_Surface *screen;
    float aspect;
    Vector globalResolutionScale;

    RenderSettings settings;
    RenderSettings active;
};


#endif

#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vector.h"
#include "RenderObject.h"

struct SDL_Surface;
class RenderObject;
class Texture;
class SDLSurfaceResource;
class Quad;
class Tile;

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

    void renderObject(const RenderObject *);
    void renderQuad(const Quad *);
    void renderSingleTexture(Texture *tex, const Vector& pos);

    // optimized function for TileGrid rendering
    void renderTileArray(Tile **tiles, unsigned int size, const Vector& start, const Vector& step);

    Texture *createTexture(const char *name);
    inline const Vector& getGlobalResolutionScale() const { return globalResolutionScale; }

    inline const RenderSettings& getSettings() const { return settings; }

protected:

    void _Enable2D(uint32 x, uint32 y);
    void _applyBlendType(BlendType blend);
    SDL_Surface *screen;
    float aspect;
    Vector globalResolutionScale;

    RenderSettings settings;


private:

    int _clientState;
    void _enableVertexAndTexCoords(); // 1
    void _disableVertexAndTexCoords(); // 1
};


#endif

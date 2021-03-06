#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Vector.h"
#include "RenderObject.h"

#include <vector>

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

    void BeginFrame();
    void EndFrame();
    unsigned int GetRenderCallCount();

    void clear();
    void show();
    void setupRenderPositionAndScale();
    void setupScreenScale();
    void loadIdentity();

    void renderObject(const RenderObject *);
    void renderQuad(const Quad *);
    void renderSingleTexture(Texture *tex, const Vector& pos);
    void renderSingleTexture(Texture *tex, const Vector& pos, const Vector& halfsize);

    // optimized function for TileGrid rendering
    void renderTileArray(Tile * const *tiles, unsigned int size, const Vector& start, const Vector& step, const Vector& halfsize);

    void render2DVertexArray(float *verts, unsigned int size);
    void drawLine(float x1, float y1, float x2, float y2, float width, float r, float g, float b, float a);
    void drawAABB(float x1, float y1, float x2, float y2, float rotation, float r, float g, float b, float a);
    void drawCircle(float x, float y, float radius, float r, float g, float b, float a);

    Texture *createTexture(const char *name);
    Texture *createNullTexture();

    unsigned int getFreeVideoMemoryKB();
    unsigned int getRenderedObjects() const { return _renderedObjects; }
    unsigned int getRenderedVerts() const { return _renderedVerts; }
    unsigned int getRenderedTiles() const { return _renderedTiles; }

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
    BlendType _activeBlend;
    unsigned int _renderedObjects;
    unsigned int _renderedVerts;
    unsigned int _renderedTiles;

    void _enableVertexAndTexCoords(); // 1
    void _disableVertexAndTexCoords(); // 1

    struct _RenderColor
    {
        float r, g, b, a;
    };

    std::vector<_RenderColor> _colorstack;

    void _multColor(float r, float g, float b, float a);
    void _pushColor();
    void _popColor();
    void _applyColor();
    inline _RenderColor& _topColor() { return _colorstack.back(); }


    // debug stuff
public:

    bool renderBorders;
};


#endif

#pragma once

#include "glmx.h"

class Image;
struct SDL_Window;
class BaseObject;
class Sprite;
class GroupObject;
class RenderLayer;

class Renderer
{
public:
    static bool StaticInit();
    static void StaticShutdown();

    Renderer(SDL_Window *win);
    ~Renderer();

    bool init();

    void beginFrame();
    void endFrame();
    void clear();
    void show();

    void destroyTex(unsigned id);
    unsigned loadTex(const Image *);

    unsigned renderObj(const glm::mat4& proj, const BaseObject *obj);
    unsigned renderSprite(glm::mat4 proj, const Sprite *obj);
    unsigned renderGroup(glm::mat4 proj, const GroupObject *obj);
    unsigned renderLayer(const RenderLayer& lr);

private:
    SDL_Window *window;
    void *glctx;
    static bool s_symbolsLoaded;
};

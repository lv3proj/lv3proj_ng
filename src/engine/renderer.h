#pragma once

#include "glm/glm.hpp"

class Image;
struct SDL_Window;

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

    unsigned getRenderCallCount();
    unsigned getFreeVideoMemoryKB();

    void destroyTex(unsigned id);
    unsigned loadTex(const Image *);

    void renderObj(const glm::mat4& proj, const BaseObject *obj);
    void renderSprite(glm::mat4 proj, const Sprite *obj);
    void renderGroup(glm::mat4 proj, const GroupObject *obj);
    void renderLayer(const RenderLayer& lr);

private:
    SDL_Window *window;
    void *glctx;
    static bool s_symbolsLoaded;
};

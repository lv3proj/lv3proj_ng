#pragma once

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

private:
    SDL_Window *window;
    void *glctx;
    static bool s_symbolsLoaded;
};

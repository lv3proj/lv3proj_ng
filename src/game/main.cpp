#include "SDLMaster.h"
#include "common.h"
//#include "Bootstrap.h"
//#include "ScriptedEngine.h"
#include "Engine.h"
#include "renderer.h"
#include "Timers.h"
#include "ResourceMgr.h"
#include "EventRecv.h"
#include "event.h"
//#include "FileAPI.h"
#include "ImguiDriver.h"
#include "LuaInterface.h"


// TEMP: TEST
void drawTestUI();

class EngineBasicWindowRecv : public WindowEventRecv
{
public:
    EngineBasicWindowRecv(EngineBase& eng, Renderer& re) : e(eng), r(re), quit(false) {}

    virtual void windowEvent(WindowEventType type)
    {
        switch(type)
        {
            case WEV_QUIT:
                quit = true;
                break;
        }

    }

    virtual void windowResize(unsigned newx, unsigned newy)
    {
    }

    EngineBase& e;
    Renderer& r;
    bool quit;
};

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_EVERYTHING);

    //Bootstrap::RelocateWorkingDir();
    //Bootstrap::HookSignals();
    //log_prepare("game_log.txt", "w");
    log_setloglevel(4);
    //Bootstrap::PrintSystemSpecs();

    srand(42); // FIXME: temporary

    if(!Renderer::StaticInit())
        return 1;
    if(!ResourceMgr::StaticInit())
        return 2;

    SDL_Window *win;
    SDL_Renderer *sdlr;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL, &win, &sdlr);
    Renderer r(win);
    r.init();
    EngineBase eng(&r);
    if(!eng.Init())
    {
        logerror("Failed to setup engine. Exiting.");
        return 3;
    }

    EventRecv evr;

    ImguiDriver *im = new ImguiDriver(win);
    if(!im->init())
        return 4;
    evr.Add(im);

    EngineBasicWindowRecv *er = new EngineBasicWindowRecv(eng, r);
    evr.Add(er);

    LuaInterface lua;
    lua.Init();

    Timer tt;
    while(!er->quit)
    {
        evr.Update();
        im->beginFrame();
        float dt = float(tt.reset()) / 1000.0f;
        eng.Update(dt);
        drawTestUI(); // <-------TEMP--------

        r.beginFrame();
            r.clear();
            eng.Render();
            im->endFrameAndRender();
        r.endFrame();
        r.show();
    }

    eng.Shutdown();

    ResourceMgr::StaticShutdown();
    Renderer::StaticShutdown();

    SDL_Quit();
    //Bootstrap::UnhookSignals();
    return 0;
}

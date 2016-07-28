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
#include "GameDevUI.h"

static bool quit = false;

class EngineBasicWindowRecv : public WindowEventRecv
{
public:
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
};

class EngineBasicKeyRecv : public KeyboardEventRecv
{
public:
    virtual void keyEvent(unsigned key, unsigned mod, unsigned state)
    {
        if(key == SDL_SCANCODE_TAB && !state)
            GameDevUI::toggle();
    }
};

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_EVERYTHING);

    //Bootstrap::RelocateWorkingDir();
    //Bootstrap::HookSignals();
    //log_prepare("game_log.txt", "w");
    log_setloglevel(4);
    //Bootstrap::PrintSystemSpecs();

    if(!Renderer::StaticInit())
        return 1;
    if(!ResourceMgr::StaticInit())
        return 2;

    EngineBase eng;
    if(!eng.Init())
    {
        logerror("Failed to setup engine. Exiting.");
        return 3;
    }

    SDL_Window *win;
    SDL_Renderer *sdlr;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL, &win, &sdlr);
    Renderer r(win);
    if(!r.init())
    {
        logerror("Failed to setup renderer. Exiting.");
        return 4;
    }

    EventRecv evr;

    ImguiDriver *im = new ImguiDriver(win);
    if(im->init())
        evr.Add(im);
    else
    {
        logerror("Failed to init ImGui driver");
        delete im;
    }

    evr.Add(new EngineBasicWindowRecv);
    evr.Add(new EngineBasicKeyRecv);

    LuaInterface lua;
    lua.Init();

    Timer tt;
    while(!quit)
    {
        evr.Update();
        if(im) im->beginFrame();
        float dt = float(tt.reset()) / 1000.0f;
        eng.Update(dt);

        GameDevUI::draw();

        r.beginFrame();
            r.clear();
            eng.Render();
            if(im) im->endFrameAndRender();
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

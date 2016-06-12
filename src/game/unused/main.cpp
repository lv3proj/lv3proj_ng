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



class EngineBasicEventRecv : public KeyboardEventRecv
{
public:
    virtual void keyEvent(unsigned key, unsigned mod, unsigned state)
    {

    }
}

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
        return 1;
    }

//#ifdef _DEBUG
    //that.
/*#else
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    that.InitScreen(info->current_w, info->current_h, 0, 0, true);
#endif*/
    Timer tt;
    EventRecv evr;
    while(!eng.IsQuit())
    {
        evr.Update();
        float dt = float(tt.reset()) / 1000.0f;
        eng.Update(dt);
    }

    eng.Shutdown();

    ResourceMgr::StaticShutdown();
    Renderer::StaticShutdown();

    SDL_Quit();
    //Bootstrap::UnhookSignals();
    return 0;
}

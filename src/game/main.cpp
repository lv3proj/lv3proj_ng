#include <SDL/SDL.h>
#include "common.h"
#include "Bootstrap.h"
#include "Engine.h"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_NOPARACHUTE);

    Bootstrap::RelocateWorkingDir();
    Bootstrap::HookSignals();
    log_prepare("game_log.txt", "w");
    log_setloglevel(4);
    Bootstrap::PrintSystemSpecs();

    EngineBase that;
    if(!that.Setup())
    {
        logerror("Failed to setup engine. Exiting.");
        return 1;
    }
    that.InitScreen(800, 600);
    that.Run();


    SDL_Quit();
    Bootstrap::UnhookSignals();
    return 0;
}

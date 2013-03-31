#include <SDL/SDL.h>
#include "common.h"
#include "Bootstrap.h"
#include "ScriptedEngine.h"
#include "FileAPI.h"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_JOYSTICK);

    //Bootstrap::RelocateWorkingDir();
    Bootstrap::HookSignals();
#ifdef _DEBUG
    log_prepare("game_log.txt", "w");
    log_setloglevel(4);
#endif
    Bootstrap::PrintSystemSpecs();

    // this should be checked, especially in larger projects
    if(!ttvfs::checkCompat())
        return 1;

    vfs.LoadFileSysRoot(false);
    vfs.Prepare();

    srand(42); // FIXME: temporary

    ScriptedEngine that;
    if(!that.Setup())
    {
        logerror("Failed to setup engine. Exiting.");
        return 1;
    }

#ifdef _DEBUG
    that.InitScreen(800, 600);
#else
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    that.InitScreen(info->current_w, info->current_h, 0, 0, true);
#endif
    that.Run();
    that.Shutdown();


    SDL_Quit();
    Bootstrap::UnhookSignals();
    return 0;
}

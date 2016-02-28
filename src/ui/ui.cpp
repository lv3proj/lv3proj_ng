#include "ui.h"
#include "renderer.h"
#include "window.h"
#include "SDLMaster.h"
#include <imgui/imgui.h>

namespace ui
{

bool Init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
        return false;

    if(!Renderer::StaticInit())
        return false;

    ImGui::GetIO().IniFilename = NULL;

    return true;
}

void Shutdown()
{
    Window::CloseAllNow();
    Renderer::StaticShutdown();
    SDL_Quit();
}



}
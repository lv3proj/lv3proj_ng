#include "ImguiDriver.h"
#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"

union SDL_Event;

ImguiDriver::ImguiDriver(SDL_Window *w)
: _win(w), _inited(false)
{
}

ImguiDriver::~ImguiDriver()
{
    if(_inited)
    {
        ImGui_Impl_GL2_Shutdown();
    }
}

bool ImguiDriver::init()
{
    _inited = false;

    if(!ImGui_ImplSdl_Init(_win))
        return false;

    if(!ImGui_Impl_GL2_CreateDeviceObjects())
        return false;

    _inited = true;
    return true;
}

void ImguiDriver::beginFrame()
{
    ImGui_ImplSdl_NewFrame(_win);
}

void ImguiDriver::endFrameAndRender()
{
    ImGui::Render();
}

void ImguiDriver::rawEvent(void *p)
{
    ImGui_ImplSdl_ProcessEvent(static_cast<SDL_Event*>(p));
}

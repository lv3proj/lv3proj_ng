#include "GameDevUI.h"
#include "imgui/imgui.h"
#include "MemoryEditor.h"
#include "Engine.h"
#include "ResourceMgr.h"

static bool perf_visible = true;
static bool test_visible = false;

static MemoryEditor medit;

static void drawMenuBar()
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("ImGui test window", NULL, &test_visible);
            ImGui::MenuItem("Perf", NULL, &perf_visible);
            ImGui::MenuItem("Memory editor", NULL, &medit.Open);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

static void drawPerfWindow()
{
    if(!perf_visible)
        return;

    if(ImGui::Begin("Perf", &perf_visible))
    {
        const float fps = ImGui::GetIO().Framerate;
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);

        ImGui::TextUnformatted("resmgr:");
        const ResourceMgr *res = g_engine->resmgr;
        unsigned n = res->getNumFrames();
        for(unsigned i = 0; i < n; ++i)
            ImGui::Text("Frame %u: %u resources", i, res->getEntriesForFrame(i));
    }
    ImGui::End();
}


static void drawWindows()
{
    if(test_visible)
        ImGui::ShowTestWindow(&test_visible);

    drawPerfWindow();

    if(medit.Open)
    {
        char thestack = false;
        medit.Draw("Memory editor", ((unsigned char*)&thestack) - 1024, 1024);
    }
}

namespace GameDevUI {

static bool main_visible = false;

void draw()
{
    if(!main_visible)
        return;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    drawMenuBar();
    drawWindows();
    ImGui::PopStyleColor(2);
}

void toggle()
{
    main_visible = !main_visible;
}

}

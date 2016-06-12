#include "imgui/imgui.h"

static bool visible = true;
static bool test = true;

void drawTestWindows()
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("ImGui test window", NULL, &test);
            ImGui::MenuItem("Hello world", NULL, &visible);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if(test)
        ImGui::ShowTestWindow(&test);

    if(visible)
    {
        if(ImGui::Begin("Hello world", &visible))
        {
            const float fps = ImGui::GetIO().Framerate;
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
        }

        ImGui::End();
    }
}

void drawTestUI()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    drawTestWindows();
    ImGui::PopStyleColor(2);
}

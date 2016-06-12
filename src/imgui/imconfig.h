//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
// This file contains compile-time options for ImGui.
// Other options (memory allocation overrides, callbacks, etc.) can be set at runtime via the ImGuiIO structure - ImGui::GetIO().
//-----------------------------------------------------------------------------

#pragma once

//#include "glmx.h"

//---- Define assertion handler. Defaults to calling assert().
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define IMGUI_API __declspec( dllexport )
//#define IMGUI_API __declspec( dllimport )

//---- Include imgui_user.inl at the end of imgui.cpp so you can include code that extends ImGui using its private data/functions.
#define IMGUI_INCLUDE_IMGUI_USER_INL

//---- Include imgui_user.h at the end of imgui.h
//#define IMGUI_INCLUDE_IMGUI_USER_H

//---- Don't implement default handlers for Windows (so as not to link with OpenClipboard() and others Win32 functions)
//#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS
//#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS

//---- Don't implement help and test window functionality (ShowUserGuide()/ShowStyleEditor()/ShowTestWindow() methods will be empty)
//#define IMGUI_DISABLE_TEST_WINDOWS

//---- Don't define obsolete functions names
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//---- Implement STB libraries in a namespace to avoid conflicts
//#define IMGUI_STB_NAMESPACE     ImGuiStb

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.

/*
#define IM_VEC2_CLASS_EXTRA                                                 \
        ImVec2(const glm::vec2& f) { x = f.x; y = f.y; }                       \
        operator glm::vec2() const { return glm::vec2(x,y); }

#define IM_VEC4_CLASS_EXTRA                                                 \
        ImVec4(const glm::vec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }     \
        operator glm::vec4() const { return glm::vec4(x,y,z,w); }
*/


//---- Freely implement extra functions within the ImGui:: namespace.
//---- Declare helpers or widgets implemented in imgui_user.inl or elsewhere, so end-user doesn't need to include multiple files.
//---- e.g. you can create variants of the ImGui::Value() helper for your low-level math types, or your own widgets/helpers.

namespace ImGui
{
    //void    Value(const char* prefix, const MyVec2& v, const char* float_format = NULL);
    //void    Value(const char* prefix, const MyVec4& v, const char* float_format = NULL);
    bool ListBoxCallback(const char* label, int* current_item, bool (*items_getter)(void*, int), void* data, int items_count, int height_in_items = -1);
    bool ShowHelpMarker(const char* desc);
    bool ComboImproved(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items = -1);
    bool Button(const char *label, bool enabled);
    bool SliderIntMulti(const char* label, int* v, int components, int *v_min, int *v_max, const char* display_format = "%.0f");
}



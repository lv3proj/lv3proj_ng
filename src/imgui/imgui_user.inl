
bool ImGui::ListBoxCallback(const char* label, int* current_item, bool (*items_getter)(void*, int), void* data, int items_count, int height_in_items)
{
    if (!ImGui::ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    bool value_changed = false;
    ImGuiListClipper clipper(items_count, ImGui::GetTextLineHeightWithSpacing());
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    {
        PushID(i);
        if(items_getter(data, i))
        {
            *current_item = i;
            value_changed = true;
        }
        PopID();
    }
    clipper.End();
    ListBoxFooter();
    return value_changed;
}

bool ImGui::ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (IsItemHovered())
    {
        SetTooltip(desc);
        return true;
    }
    return false;
}

bool ImGui::ComboImproved(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, &id))
        return false;

    const float arrow_size = (g.FontSize + style.FramePadding.x * 2.0f);
    const bool hovered = IsHovered(frame_bb, id);
    bool popup_open = IsPopupOpen(id);
    bool popup_opened_now = false;

    const ImRect value_bb(frame_bb.Min, frame_bb.Max - ImVec2(arrow_size, 0.0f));
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    RenderFrame(ImVec2(frame_bb.Max.x-arrow_size, frame_bb.Min.y), frame_bb.Max, GetColorU32(popup_open || hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button), true, style.FrameRounding); // FIXME-ROUNDING
    RenderCollapseTriangle(ImVec2(frame_bb.Max.x-arrow_size, frame_bb.Min.y) + style.FramePadding, true);

    if (*current_item >= 0 && *current_item < items_count)
    {
        const char* item_text;
        if (items_getter(data, *current_item, &item_text))
            RenderTextClipped(frame_bb.Min + style.FramePadding, value_bb.Max, item_text, NULL, NULL);
    }

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if (hovered)
    {
        SetHoveredID(id);
        if (g.IO.MouseClicked[0])
        {
            SetActiveID(0);
            if (IsPopupOpen(id))
            {
                ClosePopup(id);
            }
            else
            {
                FocusWindow(window);
                OpenPopup(label);
                popup_open = popup_opened_now = true;
            }
        }
    }

    bool value_changed = false;
    if (IsPopupOpen(id))
    {
        // Size default to hold ~7 items
        if (height_in_items < 0)
            height_in_items = 7;

        float popup_height = (label_size.y + style.ItemSpacing.y) * ImMin(items_count, height_in_items) + (style.FramePadding.y * 3);
        float popup_y1 = frame_bb.Max.y;
        float popup_y2 = ImClamp(popup_y1 + popup_height, popup_y1, g.IO.DisplaySize.y - style.DisplaySafeAreaPadding.y);
        if ((popup_y2 - popup_y1) < ImMin(popup_height, frame_bb.Min.y - style.DisplaySafeAreaPadding.y))
        {
            // Position our combo ABOVE because there's more space to fit! (FIXME: Handle in Begin() or use a shared helper. We have similar code in Begin() for popup placement)
            popup_y1 = ImClamp(frame_bb.Min.y - popup_height, style.DisplaySafeAreaPadding.y, frame_bb.Min.y);
            popup_y2 = frame_bb.Min.y;
        }
        ImRect popup_rect(ImVec2(frame_bb.Min.x, popup_y1), ImVec2(frame_bb.Max.x, popup_y2));
        SetNextWindowPos(popup_rect.Min);
        SetNextWindowSize(popup_rect.GetSize());
        PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

        const ImGuiWindowFlags flags = ImGuiWindowFlags_ComboBox | ((window->Flags & ImGuiWindowFlags_ShowBorders) ? ImGuiWindowFlags_ShowBorders : 0);
        if (BeginPopupEx(label, flags))
        {
            // Display items
            Spacing();
            for (int i = 0; i < items_count; i++)
            {
                PushID((void*)(intptr_t)i);
                const bool item_selected = (i == *current_item);
                const char* item_text;
                if (items_getter(data, i, &item_text) && item_text && ImGui::Selectable(item_text, item_selected)) // <--- THIS LINE MODIFIED
                {
                    SetActiveID(0);
                    value_changed = true;
                    *current_item = i;
                }
                if (item_selected && popup_opened_now)
                    SetScrollHere();
                PopID();
            }
            EndPopup();
        }
        PopStyleVar();
    }
    return value_changed;
}

bool ImGui::Button(const char *label, bool enabled)
{
    if(!enabled)
    {
        ImVec4 col(0.3f, 0.3f, 0.4f, 1.0f);
        PushStyleColor(ImGuiCol_Button, col);
        PushStyleColor(ImGuiCol_ButtonHovered, col);
    }
    bool ret = ButtonEx(label, ImVec2(0, 0), enabled ? 0 : ImGuiButtonFlags_Disabled);
    if(!enabled)
        PopStyleColor(2);
    return ret;
}


bool ImGui::SliderIntMulti(const char* label, int* v, int components, int *v_min, int *v_max, const char* display_format)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= SliderInt("##v", &v[i], v_min[i], v_max[i], display_format);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
    }
    PopID();

    TextUnformatted(label, FindRenderedTextEnd(label));
    EndGroup();

    return value_changed;
}
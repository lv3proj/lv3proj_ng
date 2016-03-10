#include "window.h"
#include "renderer.h"
#include "SDLMaster.h"
#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "event.h"

#include <stdio.h>

Window *Window::s_windowBySDLWindowID[] = { NULL };
Window *Window::s_activeWindow = NULL;

#define PENDING ((SDL_Event*)pendingEvents)


Window::Window()
: render(NULL)
, window(NULL)
, _closing(false)
, _autoclose(true)
, _full(false)
, _originalW(0), _originalH(0)
, pendingEvents(NULL)
, pendingEventsCapacity(0)
, numPendingEvents(0)
{
}

Window::~Window()
{
    close();
}

void Window::close()
{
    onClose();
    delete render;
    if(window)
    {
        s_windowBySDLWindowID[SDL_GetWindowID(window)] = NULL;
        if(s_activeWindow == this)
            s_activeWindow = NULL;
        SDL_DestroyWindow(window);
    }
    render = NULL;
    window = NULL;
    _closing = false;
}


bool Window::open(unsigned w, unsigned h, bool full)
{
    //printf("Window::open() on display #%u [%s]\n", displayIdx, SDL_GetDisplayName(displayIdx));

    _closing = false;
    bool autoWH = false;

    if(w && h)
    {
        _originalW = w;
        _originalH = h;
    }
    else
    {
       int disp = window ? SDL_GetWindowDisplayIndex(window) : 0;
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(disp, &mode);
        w = mode.w;
        h = mode.h;
        printf("Detected mode: %ux%u on display #%u [%s]\n", w, h, disp, SDL_GetDisplayName(disp));
        autoWH = true;
    }
    if(!_originalW)
        _originalW = w;
    if(!_originalH)
        _originalH = h;

    unsigned flags = SDL_WINDOW_OPENGL;
    if(full)
    {
        if(autoWH)
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        else
            flags |= SDL_WINDOW_FULLSCREEN;
    }
    else
        flags |= SDL_WINDOW_RESIZABLE;

    if(window)
    {
        int disp = SDL_GetWindowDisplayIndex(window);
        printf("Toggle fullscreen on display #%u\n", disp);
        flags &= (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
        if(full)
            SDL_SetWindowSize(window, w, h);
        if(SDL_SetWindowFullscreen(window, flags) < 0)
            return false;
        if(!full)
            SDL_SetWindowSize(window, _originalW, _originalH);
    }
    else
    {
        window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);
        if(!window)
            return false;

        s_windowBySDLWindowID[SDL_GetWindowID(window)] = this;

        render = new Renderer(window);
        if(!render || !render->init())
        {
            close();
            return false;
        }

        ImGui_ImplSdl_Init(window);

        onOpen();
    }

    _full = full;

    if(const size_t sz = _ev.size())
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        for(size_t i = 0; i < sz; ++i)
            _ev[i]->windowResize(w, h);
    }

    return true;
}

void Window::RequestCloseAll()
{
    for(unsigned i = 0; i < MAX_WINDOWS; ++i)
        if(Window *w = s_windowBySDLWindowID[i])
            w->_closing = true;
}

void Window::CloseAllNow()
{
    for(unsigned i = 0; i < MAX_WINDOWS; ++i)
        if(Window *w = s_windowBySDLWindowID[i])
            w->close();
}

bool Window::IsAnyOpen()
{
    for(unsigned i = 0; i < MAX_WINDOWS; ++i)
        if(s_windowBySDLWindowID[i])
            return true;
    return false;
}

void Window::HandleEvents(void)
{
    for(unsigned i = 0; i < MAX_WINDOWS; ++i)
        if(Window *w = s_windowBySDLWindowID[i])
            if(w->_closing && w->_autoclose)
                w->close();

    SDL_Event evt;
    while(SDL_PollEvent(&evt))
    {
        unsigned winID = 0;
        switch(evt.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                winID = evt.key.windowID;
                break;
            case SDL_WINDOWEVENT:
                winID = evt.window.windowID;
                break;
            case SDL_MOUSEMOTION:
                winID = evt.motion.windowID;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                winID = evt.button.windowID;
                break;
            case SDL_MOUSEWHEEL:
                winID = evt.wheel.windowID;
                break;

            case SDL_QUIT:
                for(unsigned i = 0; i < MAX_WINDOWS; ++i)
                    if(Window *w = s_windowBySDLWindowID[i])
                        w->_closing = true;
                break;
        }

        Window *target = winID ? s_windowBySDLWindowID[winID] : s_activeWindow;
        if(target)
            target->_forwardEvent(evt);
    }
}

void Window::_forwardEvent(const SDL_Event& evt)
{
    switch(evt.type)
    {
        case SDL_WINDOWEVENT:
            switch(evt.window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                _closing = true;
                break;
            }
        break;

        case SDL_KEYDOWN:
            switch(evt.key.keysym.sym)
            {
                case SDLK_RETURN:
                    if(evt.key.keysym.mod & KMOD_ALT)
                        open(0, 0, !_full);
                    break;
            }
        break;
    }
    _pushPendingEvent(evt);

    // ------ Only with listener from here --------
    const size_t sz = _ev.size();
    if(!sz)
        return;
    switch(evt.type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            for(size_t i = 0; i < sz; ++i)
                _ev[i]->keyEvent(evt.key.keysym.scancode, evt.key.keysym.mod, evt.key.state);
            break;

        case SDL_WINDOWEVENT:
            switch(evt.window.event)
            {
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    for(size_t i = 0; i < sz; ++i)
                        _ev[i]->windowEvent(WEV_FOCUS);
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    for(size_t i = 0; i < sz; ++i)
                        _ev[i]->windowEvent(WEV_LOSTFOCUS);
                    break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    for(size_t i = 0; i < sz; ++i)
                        _ev[i]->windowResize(evt.window.data1, evt.window.data2);
                    break;
            }
            break;

        case SDL_MOUSEMOTION:
            for(size_t i = 0; i < sz; ++i)
                _ev[i]->mouseMoved(evt.motion.x, evt.motion.y, evt.motion.xrel, evt.motion.yrel, evt.motion.state);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            for(size_t i = 0; i < sz; ++i)
                _ev[i]->mouseClick(evt.button.button, evt.button.state, evt.button.x, evt.button.y);
            break;

        case SDL_MOUSEWHEEL:
            for(size_t i = 0; i < sz; ++i)
                _ev[i]->mouseWheel(evt.wheel.y);
            break;
    }
}

void Window::_pushPendingEvent(const SDL_Event& evt)
{
    const unsigned n = numPendingEvents;
    const unsigned cap = pendingEventsCapacity;
    if(n+1 > cap)
    {
        pendingEvents = realloc(pendingEvents, sizeof(SDL_Event) * (cap + 32));
        assert(pendingEvents);
        pendingEventsCapacity = cap + 32;
    }
    PENDING[n] = evt;
    numPendingEvents = n+1;
}


bool Window::isMouseButton(unsigned int btn)
{
    unsigned b = SDL_GetMouseState(NULL, NULL);
    return !!(b & SDL_BUTTON(btn));
}

bool Window::isKey(int k) const
{
    int numkeys = 0;
    const unsigned char *kb = SDL_GetKeyboardState(&numkeys);
    return k < numkeys && !!kb[k];
}

void Window::draw()
{
    // process events for imgui just before we are about to render,
    // since we only want to affect the window in question
    if(const unsigned npend = numPendingEvents)
    {
        numPendingEvents = 0;
        for(unsigned i = 0; i < npend; ++i)
            ImGui_ImplSdl_ProcessEvent(&PENDING[i]);
    }

    if(!render)
        return;

    render->beginFrame();
    render->clear();

    ImGui_ImplSdl_NewFrame(window);

    for(size_t i = 0; i < _drawHooks.size(); ++i)
        _drawHooks[i].fn(render, _drawHooks[i].data);

    ImGui::Render();

    render->endFrame();
    render->show();
}

void Window::addDrawHook(Window::DrawHookFn fn, void *ud)
{
    DrawHook h = { fn, ud };
    _drawHooks.push_back(h);
}

void Window::getSize(int *w, int *h) const
{
    if(window)
        SDL_GetWindowSize(window, w, h);
    else
    {
        if(w)
            *w = 0;
        if(h)
            *h = 0;
    }
}

void Window::setTitle(const char *s)
{
    if(window)
        SDL_SetWindowTitle(window, s);
}

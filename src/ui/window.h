#pragma once

#include <vector>

class Renderer;
class Texture;
class IEventRecv;

typedef union SDL_Event SDL_Event;
struct SDL_Window;


class Window
{
public:
    enum { MAX_WINDOWS = 32 };

    static void HandleEvents();
    static void RequestCloseAll();
    static void CloseAllNow();
    static bool IsAnyOpen();

    Window();
    ~Window();
    bool open(unsigned w, unsigned h, bool full = false);
    void close();

    void draw();

    bool isKey(int k) const; // SDL_Scancode
    void getSize(int *w, int *h) const;

    inline Renderer *getRenderer() const { return render; }

    bool isMouseButton(unsigned int btn);

    bool isOpen() const { return !!window; }
    bool isClosing() const { return _closing; }

    typedef void (*DrawHookFn)(Renderer *, void *);
    void addDrawHook(DrawHookFn fn, void *ud);

    void addEventRecv(IEventRecv *ev) { _ev.push_back(ev); }
    void setTitle(const char *s);

protected:
    virtual void onOpen() {}
    virtual void onClose() {}

    struct DrawHook
    {
        DrawHookFn fn;
        void *data;
    };
    void _forwardEvent(const SDL_Event& ev);
    void _pushPendingEvent(const SDL_Event&);
    
    Renderer *render;
    SDL_Window *window;
    std::vector<DrawHook> _drawHooks;
    std::vector<IEventRecv*> _ev;
    bool _closing, _autoclose, _full;
    unsigned _originalW, _originalH;

    void *pendingEvents;
    unsigned pendingEventsCapacity;
    unsigned numPendingEvents;

    static Window *s_windowBySDLWindowID[MAX_WINDOWS];
    static Window *s_activeWindow;
};

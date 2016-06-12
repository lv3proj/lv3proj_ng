#include "EventRecv.h"

#include "glmx.h"
#include "SDLMaster.h"
#include "event.h"


EventRecv::EventRecv()
{
}

EventRecv::~EventRecv()
{
    Clear();
}

template<typename T>
static void _Clear(T& v)
{
    for(size_t i = 0; i < v.size(); ++i)
        delete v[i];
    v.clear();
}

void EventRecv::Clear()
{
    _Clear(mouse);
    _Clear(kb);
    _Clear(win);
    _Clear(joy);
    _Clear(raw);
}

#define FORALL(v, mth) for(size_t i = 0; i < v.size(); ++i) v[i]->mth

void EventRecv::Update()
{
    SDL_Event ev;
    while(SDL_PollEvent(&ev))
    {
        FORALL(raw, rawEvent)(&ev);

        switch(ev.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                FORALL(kb, keyEvent)(ev.key.keysym.sym, ev.key.keysym.mod, ev.key.state);
                break;

            case SDL_JOYAXISMOTION:
            {
                const float val = glm::clamp((float)ev.jaxis.value / 32767.0f, -1.0f, 1.0f);
                FORALL(joy, axisChange)(ev.jaxis.which, ev.jaxis.axis, val);
                break;
            }

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                FORALL(joy, button)(ev.jbutton.which, ev.jbutton.button, ev.jbutton.state);
                break;

            /*case SDL_JOYHATMOTION:
                FORALL(joy, hat)(ev.jhat.which, ev.jhat.hat, 
                OnJoystickEvent(ev.jhat.type, ev.jhat.which, ev.jhat.hat, ev.jhat.value);
                break; */

            case SDL_MOUSEMOTION:
                FORALL(mouse, mouseMoved)(ev.motion.which, ev.motion.x, ev.motion.y, ev.motion.xrel, ev.motion.yrel, ev.motion.state);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                FORALL(mouse, mouseClick)(ev.button.which, ev.button.button, ev.button.state, ev.button.x, ev.button.y);
                break;

            case SDL_QUIT:
                FORALL(win, windowEvent)(WEV_QUIT);
                break;

            case SDL_WINDOWEVENT:
            {
                switch(ev.window.event)
                {
                    case SDL_WINDOWEVENT_CLOSE:
                        FORALL(win, windowEvent)(WEV_QUIT);
                        break;

                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        FORALL(win, windowResize)(ev.window.data1, ev.window.data2);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        FORALL(win, windowEvent)(WEV_FOCUS);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        FORALL(win, windowEvent)(WEV_LOSTFOCUS);
                        break;
                }

                break;
            }

        }
    }
}



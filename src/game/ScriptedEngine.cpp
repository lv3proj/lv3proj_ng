#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Renderer.h"
#include "ScriptedEntity.h"

#include "GL/gl.h"
#include "ObsRender.h"

ScriptedEngine *scriptedEngine = NULL;

ScriptedEngine::ScriptedEngine()
 : script(NULL), _obsRender(NULL)
{
    scriptedEngine = this;
}

ScriptedEngine::~ScriptedEngine()
{
    scriptedEngine = NULL;
}

void ScriptedEngine::Shutdown()
{
    _obsRender->kill();
    _obsRender = NULL;

    EngineBase::Shutdown();
}

bool ScriptedEngine::OnInit()
{
    if(!EngineBase::OnInit())
        return false;

    script = new LuaInterface();
    script->Init();
    script->call("onInit");

    _CreateInternalObjects();

    return true;
}

void ScriptedEngine::OnReset()
{
    _obsRender = NULL;
    EngineBase::OnReset();
    script->Shutdown();
    script->Init();
    script->call("onInit");

    _CreateInternalObjects();
}

void ScriptedEngine::_CreateInternalObjects()
{
    if(!_obsRender)
    {
        _obsRender = new ObsGridRender;
        _obsRender->alpha = 0;
        _obsRender->color = Vector(1, 0, 0);
        layers->GetLayer(20)->Add(_obsRender);
        objmgr->AddObject(_obsRender);
    }
}

void ScriptedEngine::OnUpdate(float dt)
{
    EngineBase::OnUpdate(dt);
    script->call("onUpdate", dt);
}

void ScriptedEngine::OnRender()
{
    EngineBase::OnRender();

    script->call("onRender");
}

void ScriptedEngine::UnregisterObject(ScriptObject *obj)
{
    EngineBase::UnregisterObject(obj);

    if(obj->scriptBindings)
        script->UnregisterObject(obj);
}

void ScriptedEngine::ClearGarbage(bool deep)
{
    EngineBase::ClearGarbage(deep);
    if(deep)
    {
        script->GC();
    }
}

void ScriptedEngine::OnKeyDown(SDLKey key, SDLMod mod)
{
    EngineBase::OnKeyDown(key, mod);

    script->call("onKeyDown", key, mod);

    if(key == SDLK_F9)
    {
        if(_obsRender->alpha.x <= 0)
            _obsRender->alpha.interpolateTo(0.5f, 0);
        else
            _obsRender->alpha.interpolateTo(0, 0);
    }
    else if(key == SDLK_F8)
    {
        Entity::s_renderCollisionShapes = !Entity::s_renderCollisionShapes;
    }
    else if(key == SDLK_F5 && (mod & KMOD_CTRL))
    {
        SetReset(true);
    }
    else if(key == SDLK_F10)
    {
        GetRenderer()->renderBorders = !GetRenderer()->renderBorders;
    }
}

void ScriptedEngine::OnKeyUp(SDLKey key, SDLMod mod)
{
    script->call("onKeyUp", key, mod);
}

void ScriptedEngine::OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val)
{
    script->call("onJoystickEvent", device, type, id, val);
}


// Handle a raw SDL_Event before anything else. return true for further processing,
// false to drop the event and NOT pass it to other On..Event functions
bool EngineBase::OnRawEvent(SDL_Event& evt)
{
    return true;
}

void EngineBase::OnWindowEvent(bool active)
{
    // TODO: pause if lost focus?
}

void EngineBase::OnMouseEvent(uint32 type, uint32 button, uint32 state, uint32 x, uint32 y, int32 rx, int32 ry)
{
    const RenderSettings &rr = render->getSettings();
    Vector lastMouseWinPos = mouse.winPos;

    mouse.winPos.x = rangeTransform<float>(x, 0, rr.pixelsW, -virtualOffX, rr.virtualW + virtualOffX);
    mouse.winPos.y = rangeTransform<float>(y, 0, rr.pixelsH, -virtualOffY, rr.virtualH + virtualOffY);

    mouse.winRel = mouse.winPos - lastMouseWinPos;

    mouse.worldPos = ToWorldPosition(mouse.winPos);
    mouse.worldRel = ToWorldScale(mouse.winRel);

    if(state)
    {
        mouse.buttons |= SDL_BUTTON(button);

        // This is reset at the beginning of input handling.
        // SDL always sends 2 events for wheel movement, one with state 1, and the next one with state 0.
        // We only handle one of them.
        switch(button)
        {
        case SDL_BUTTON_WHEELUP:
            ++mouse.wheelRel;
            break;

        case SDL_BUTTON_WHEELDOWN:
            --mouse.wheelRel;
            break;
        }
    }
    else
    {
        mouse.buttons &= ~SDL_BUTTON(button);
    }
}

void EngineBase::OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val)
{
}

void EngineBase::OnKeyDown(SDLKey key, SDLMod mod)
{
    if(mod & KMOD_LALT)
    {
        if(key == SDLK_F4)
            SetQuit(true);
        //else if(key == SDLK_RETURN)
        //    ToggleFullscreen();
    }

    if(key == SDLK_PAUSE)
    {
        TogglePause();
    }
}

void EngineBase::OnKeyUp(SDLKey key, SDLMod mod)
{
}

void EngineBase::OnWindowResize(uint32 newx, uint32 newy)
{
    InitScreen(newx, newy);
}

void EngineBase::OnLog(const char *, int color, int level, void *user)
{
}

void EngineBase::OnUpdate(float dt)
{
}

void EngineBase::OnReset()
{
    _pause = 0;
}

void EngineBase::OnRender()
{
}

bool EngineBase::IsMouseButton(unsigned int btn)
{
    return mouse.buttons & SDL_BUTTON(btn);
}


void EngineBase::_ProcessEvents(void)
{
    mouse.worldRel = mouse.winRel = Vector(0, 0);
    mouse.wheelRel = 0;

    SDL_Event evt;
    while(!s_quit && SDL_PollEvent(&evt))
    {
        if(!OnRawEvent(evt))
            continue;

        switch(evt.type)
        {
        case SDL_KEYDOWN:
            OnKeyDown(evt.key.keysym.sym, evt.key.keysym.mod);
            break;

        case SDL_KEYUP:
            OnKeyUp(evt.key.keysym.sym, evt.key.keysym.mod);
            break;

        case SDL_JOYAXISMOTION:
            OnJoystickEvent(evt.jaxis.type, evt.jaxis.which, evt.jaxis.axis, evt.jaxis.value);
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            OnJoystickEvent(evt.jbutton.type, evt.jbutton.which, evt.jbutton.button, evt.jbutton.state);
            break;

        case SDL_JOYHATMOTION:
            OnJoystickEvent(evt.jhat.type, evt.jhat.which, evt.jhat.hat, evt.jhat.value);
            break;

        case SDL_ACTIVEEVENT:
            OnWindowEvent(evt.active.gain);
            break;

        case SDL_VIDEORESIZE:
            OnWindowResize(evt.resize.w, evt.resize.h);
            break;

        case SDL_MOUSEMOTION:
            OnMouseEvent(evt.type, 0, evt.motion.state, evt.motion.x, evt.motion.y, evt.motion.xrel, evt.motion.yrel);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouseEvent(evt.type, evt.button.button, evt.button.state, evt.button.x, evt.button.y, 0, 0);
            break;
        }
    }
}


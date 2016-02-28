#ifndef SCRIPTED_ENGINE_H
#define SCRIPTED_ENGINE_H

#include "Engine.h"

class LuaInterface;
class ObsGridRender;

class InputHandler
{
    virtual bool OnInit();
    virtual void OnReset();
    virtual void OnRender();
    virtual void OnUpdate(float dt);
    virtual void OnMouseEvent(uint32 type, uint32 button, uint32 state, uint32 x, uint32 y, int32 rx, int32 ry);
    virtual void OnKeyDown(SDLKey key, SDLMod mod);
    virtual void OnKeyUp(SDLKey key, SDLMod mod);
    virtual void OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val);
    virtual void OnWindowEvent(bool active);
    virtual void OnWindowResize(uint32 newx, uint32 newy);
    virtual bool OnRawEvent(SDL_Event& evt); // return true to pass this event to the following internal event handlers, false to proceed with next event
};


class ScriptedEngine : public EngineBase
{
public:

    ScriptedEngine();
    virtual ~ScriptedEngine();

    LuaInterface *script;

    virtual void UnregisterObject(ScriptObject *obj);
    virtual void ClearGarbage(bool deep);
    virtual void Shutdown();

protected:

    virtual bool OnInit();
    virtual void OnReset();
    virtual void OnUpdate(float dt);
    virtual void OnRender();
    virtual void OnKeyDown(SDLKey key, SDLMod mod);
    virtual void OnKeyUp(SDLKey key, SDLMod mod);
    virtual void OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val);



    void _CreateInternalObjects();

    ObsGridRender *_obsRender;

};

extern ScriptedEngine *scriptedEngine;


#endif

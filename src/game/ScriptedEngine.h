#ifndef SCRIPTED_ENGINE_H
#define SCRIPTED_ENGINE_H

#include "Engine.h"

class LuaInterface;
class ObsGridRender;


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
};

extern ScriptedEngine *g_scriptedEngine;


#endif

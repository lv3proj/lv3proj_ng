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

    ObsGridRender *_obsRender;

};

extern ScriptedEngine *scriptedEngine;


#endif

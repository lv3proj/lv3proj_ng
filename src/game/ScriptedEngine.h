#ifndef SCRIPTED_ENGINE_H
#define SCRIPTED_ENGINE_H

#include "Engine.h"

class LuaInterface;

class ScriptedEngine : public EngineBase
{
public:

    ScriptedEngine();
    virtual ~ScriptedEngine();

    LuaInterface *script;

    virtual void UnregisterObject(ScriptObject *obj);
    virtual void ClearGarbage(bool deep);

protected:

    virtual bool OnInit();
    virtual void OnReset();
    virtual void OnUpdate(float dt);

};


#endif

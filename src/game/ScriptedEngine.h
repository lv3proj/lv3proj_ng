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

protected:

    virtual bool OnInit();
    virtual void OnReset();
    virtual void OnUpdate(float dt);

};


#endif

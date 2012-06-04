#include "ScriptedEngine.h"
#include "LuaInterface.h"

ScriptedEngine::ScriptedEngine()
 : script(NULL)
{
}

ScriptedEngine::~ScriptedEngine()
{
}

bool ScriptedEngine::OnInit()
{
    if(!EngineBase::OnInit())
        return false;

    script = new LuaInterface();
    if(!script->Init())
        return false;

    return true;
}

void ScriptedEngine::OnReset()
{
    EngineBase::OnReset();
}

void ScriptedEngine::OnUpdate(float dt)
{
    EngineBase::OnUpdate(dt);


}


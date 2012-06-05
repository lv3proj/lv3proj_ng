#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"

#include "TestRenderObject.h"

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

    TestRenderObject *ro = new TestRenderObject();
    ro->setTexture("test.png");
    objmgr->AddObject(ro);

    script->call("onInit");

    return true;
}

void ScriptedEngine::OnReset()
{
    EngineBase::OnReset();
}

void ScriptedEngine::OnUpdate(float dt)
{
    EngineBase::OnUpdate(dt);
    script->call("onUpdate", dt);
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



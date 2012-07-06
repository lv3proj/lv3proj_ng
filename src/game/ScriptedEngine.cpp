#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Renderer.h"

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
    if(!script->Init())
        return false;

    script->call("onInit");

    /*ObsRender *o = new ObsRender(4);
    o->alpha = 0.4f;
    o->color = Vector(1, 0, 0);
    layers->GetLayer(4)->Add(o);
    objmgr->AddObject(o);*/

    _obsRender = new ObsGridRender;
    _obsRender->alpha = 0;
    _obsRender->color = Vector(1, 0, 0);
    layers->GetLayer(20)->Add(_obsRender);
    objmgr->AddObject(_obsRender);

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

    if(key == SDLK_F9)
    {
        if(_obsRender->alpha.x <= 0)
            _obsRender->alpha.interpolateTo(0.5f, 0);
        else
            _obsRender->alpha.interpolateTo(0, 0);
    }
}

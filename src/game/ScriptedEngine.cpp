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


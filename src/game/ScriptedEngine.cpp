#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Renderer.h"

#include "GL/gl.h"
#include "ObsRender.h"
#include "TestRenderObject.h"

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

    layers->GetLayer(4)->tiles.SetSize(64);
    layers->GetLayer(4)->tiles.colliding = true;
    layers->GetLayer(3)->tiles.SetSize(64);
    layers->GetLayer(3)->tiles.colliding = true;
    char buf[20];
    for(unsigned int y = 0; y < 64; ++y)
    {
        layers->GetLayer(4)->tiles.SetTileByName(0, y, "block4.png");
        for(unsigned int x = 1; x < 64; ++x)
        {
            if(rand() & 1)
                continue;
            /*if(rand() % 6 == 0)
            {
                layers->GetLayer(4)->tiles.SetTileByName(x, y, "en.anim");
            }
            else*/
            {
                sprintf(buf, "block%u.png", (rand() % 4) + 1);
                layers->GetLayer(4)->tiles.SetTileByName(x, y, &buf[0]);
            }
        }
    }

    for(unsigned int y = 0; y < 64; ++y)
        for(unsigned int x = 0; x < 64; ++x)
        {
            layers->GetLayer(3)->tiles.SetTileByName(x, y, "blocktest.png");
        }

    /*ObsRender *o = new ObsRender(4);
    o->alpha = 0.4f;
    o->color = Vector(1, 0, 0);
    layers->GetLayer(4)->Add(o);
    objmgr->AddObject(o);*/

    obsgrid.Init(64, 16);
    obsgrid.Setup();

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

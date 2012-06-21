#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Renderer.h"

#include "GL/gl.h"
#include "ObsRender.h"
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

    script->call("onInit");

    layers->GetLayer(4)->tiles.SetSize(64);
    char buf[20];
    for(unsigned int y = 0; y < 64; ++y)
        for(unsigned int x = 0; x < 64; ++x)
        {
            if(rand() & 1)
                continue;
            if(rand() % 6 == 0)
            {
                layers->GetLayer(4)->tiles.SetTileByName(x, y, "en.anim");
                layers->GetLayer(4)->tiles.GetTile(x, y)->CalcCollision();
            }
            else
            {
                sprintf(buf, "block%u.png", (rand() % 4) + 1);
                layers->GetLayer(4)->tiles.SetTileByName(x, y, &buf[0]);
                layers->GetLayer(4)->tiles.GetTile(x, y)->CalcCollision();
            }
        }

    ObsRender *o = new ObsRender(4);
    o->alpha = 0.4f;
    o->color = Vector(1, 0, 0);
    layers->GetLayer(4)->Add(o);
    objmgr->AddObject(o);

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



#include "ScriptedEngine.h"
#include "LuaInterface.h"
#include "ObjectMgr.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Renderer.h"
#include "GL/gl.h"

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
            if(rand() % 6 == 0)
            {
                layers->GetLayer(4)->tiles.SetTileByName(x, y, "en.anim");
            }
            else
            {
                sprintf(buf, "block%u.png", (rand() % 3) + 1);
                layers->GetLayer(4)->tiles.SetTileByName(x, y, &buf[0]);
            }
        }

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



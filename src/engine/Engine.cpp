#include "Engine.h"
#include "renderer.h"
//#include "SoundCore.h"
#include "ResourceMgr.h"
#include "Quad.h"
#include "Texture.h"
#include "ObjectMgr.h"
#include "Camera.h"
#include "RenderLayerMgr.h"
#include "TileMgr.h"

EngineBase::EngineBase(Renderer *r)
: camera(NULL)
, virtualOffX(0)
, virtualOffY(0)
//, sound(NULL)
, layers(NULL)
, resmgr(NULL)
, render(r)
, _pause(0)
{
    log("Game Engine start.");
}

EngineBase::~EngineBase()
{
    delete layers;
    delete camera;
    delete objmgr;
    //delete sound;
    delete tiles;
    delete resmgr;
}

bool EngineBase::Init(void)
{
    //sound = new SoundCore();
    //if(!sound->Init())
    //    return false;
    //log_setcallback(&s_OnLog, true, NULL); // TODO: param? newline?

    resmgr = new ResourceMgr();
    objmgr = new ObjectMgr();
    layers = new RenderLayerMgr();
    camera = new Camera();
    tiles = new TileMgr();

    resmgr->pushFrame();

    return true;
}

void EngineBase::Shutdown(void)
{
    obsgrid.Clear();
    layers->ClearAll();
    tiles->DeleteUnused();
    if(tiles->GetNumTiles())
        logerror("TileMgr: %u tiles not deleted", tiles->GetNumTiles());
    objmgr->ClearAll();
    //sound->Shutdown();
    ClearGarbage(true);

    // be sure we did a clean shutdown
    //resMgr.DbgCheckEmpty();
}

void EngineBase::Update(float dt)
{
    camera->update(dt);
    objmgr->Update(dt);

    ClearGarbage(false);
}

void EngineBase::Render()
{
    camera->update(0);
    layers->Render(render);
}


void EngineBase::Reset()
{
    logdetail("EngineBase: Reset!");
    _reset = false;
    layers->ClearAll();
    objmgr->ClearAll();
    ClearGarbage(true);
}

CountedPtr<Texture> EngineBase::GetTexture(const char *name)
{
    CountedPtr<Texture> tex = resmgr->_getTex(name);
    if(!tex)
    {

    }
    return tex;
}

void EngineBase::UnregisterObject(ScriptObject *obj)
{
    if(obj->isManaged())
        objmgr->Garbage(obj);
}

void EngineBase::ClearGarbage(bool deep)
{
    obsgrid.OptimizeIncremental();
    objmgr->ClearGarbage();
    if(deep)
    {
        //sound->ClearGarbage();
        resmgr->popFrame();
        resmgr->pushFrame();
    }
}

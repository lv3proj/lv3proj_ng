#ifndef ENGINE_H
#define ENGINE_H

#include "common.h"
//#include "ObsGrid.h"
#include "MouseState.h"
#include "refcounted.h"

class Renderer;
//class SoundCore;
class ScriptObject;
class ObjectMgr;
class RenderLayerMgr;
class Camera;
class TileMgr;
class ResourceMgr;


/*
class EventHandler
{
    virtual bool OnInit();
    virtual void OnReset();
    virtual void OnRender();
    virtual void OnUpdate(float dt);
    virtual void OnMouseEvent(uint32 type, uint32 button, uint32 state, uint32 x, uint32 y, int32 rx, int32 ry);
    virtual void OnKeyDown(SDLKey key, SDLMod mod);
    virtual void OnKeyUp(SDLKey key, SDLMod mod);
    virtual void OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val);
    virtual void OnWindowEvent(bool active);
    virtual void OnWindowResize(uint32 newx, uint32 newy);
    virtual bool OnRawEvent(SDL_Event& evt); // return true to pass this event to the following internal event handlers, false to proceed with next event
};
*/

class EngineBase
{
public:
    EngineBase(Renderer *r);
    virtual ~EngineBase();

    bool Init(void);
    void Shutdown(void);
    void Update(float dt);
    void Render();

    inline void SetReset(bool r = true) { _reset = r; }
    inline bool IsReset(void) { return _reset; }
    void Reset();

    MouseState mouse;

    //Camera *camera;
    float virtualOffX;
    float virtualOffY;
    
    //ObsGrid obsgrid;
    //SoundCore *sound;
    //ObjectMgr *objmgr;
    //RenderLayerMgr *layers;
    //TileMgr *tiles;
    ResourceMgr *resmgr;

    //virtual void UnregisterObject(ScriptObject *obj);
    //virtual void ClearGarbage(bool deep);

    inline bool IsPause(int level = 0) const { return level < _pause; }
    inline void SetPause(int level) { _pause = level; }
    inline int GetPause() const { return _pause; }

protected:
    Renderer *_r;
    bool _reset;
    int _pause;
};

extern EngineBase *g_engine;

#endif

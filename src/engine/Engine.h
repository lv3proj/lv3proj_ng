#ifndef ENGINE_H
#define ENGINE_H

#include "common.h"
#include "Vector.h"
#include "ObsGrid.h"
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


class EngineBase
{
public:
    EngineBase(Renderer *r);
    virtual ~EngineBase();

    virtual bool Init(void);
    virtual void Shutdown(void);
    virtual void Update(float dt);
    virtual void Render();

    void SetTitle(const char *title);
    inline void SetReset(bool r = true) { _reset = r; }
    inline bool IsReset(void) { return _reset; }
    void Reset();

    MouseState mouse;

    Camera *camera;
    float virtualOffX;
    float virtualOffY;
    
    ObsGrid obsgrid;
    //SoundCore *sound;
    ObjectMgr *objmgr;
    RenderLayerMgr *layers;
    TileMgr *tiles;

    virtual void UnregisterObject(ScriptObject *obj);
    virtual void ClearGarbage(bool deep);

    bool IsMouseButton(unsigned int btn);
    void SetMousePos(const Vector& pos);

    Vector ToWorldPosition(const Vector& v) const; // window -> world
    Vector ToWindowPosition(const Vector& v) const; // world -> window

    Vector ToWorldScale(const Vector& v) const; // window -> world
    Vector ToWindowScale(const Vector& v) const; // world -> window

    Vector GetCameraPositionFor(const Vector &pos);

    inline bool IsPause(int32 level = 0) const { return level < _pause; }
    inline void SetPause(int32 level) { _pause = level; }
    inline int32 GetPause() const { return _pause; }

protected:

    ResourceMgr *resmgr;
    Renderer *render;

    bool _reset;
    bool _quit;
    int32 _pause;
};

#endif

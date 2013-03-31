#ifndef ENGINE_H
#define ENGINE_H

#include "common.h"
#include <SDL/SDL.h>
#include "Vector.h"
#include "ObsGrid.h"

class Renderer;
class SoundCore;
class Texture;
class ScriptObject;
class ObjectMgr;
class RenderLayerMgr;
class Camera;

struct MouseState
{
    MouseState() : active(true), buttons(0), wheelAbs(0), wheelRel(0) {}
    bool active;
    Vector worldPos;
    Vector worldRel;
    Vector winPos;
    Vector winRel;
    unsigned int buttons;
    int wheelAbs;
    int wheelRel;

    inline bool isLeft() const { return buttons & SDL_BUTTON_LMASK; }
    inline bool isRight() const { return buttons & SDL_BUTTON_RMASK; }
    inline bool isMiddle() const { return buttons & SDL_BUTTON_MMASK; }
};

class EngineBase
{
public:
    EngineBase();
    virtual ~EngineBase();

    inline static bool IsQuit(void) { return s_quit; }
    inline static void SetQuit(bool q = true) { s_quit = q; }
    inline static SDL_Joystick *GetJoystick(uint32 i) { return i < s_joysticks.size() ? s_joysticks[i] : NULL; }
    inline static uint32 GetJoystickCount(void) { return s_joysticks.size(); }
    inline static void SetSpeed(float s, float t = 0, int loop = 0, bool pingpong = false, bool ease = false)
    {
        s_speed.interpolateTo(s, t, loop, pingpong, ease);
    }
    inline static float GetSpeed(void) { return s_speed.x; }
    inline static uint32 GetCurFrameTime(void) { return s_curFrameTime; }
    inline static double GetCurFrameTimeF(void) { return (s_curFrameTime + (double)s_accuTime) / 1000.0; } // float is not precise enough, as this can get very high
    inline static uint32 GetTimeDiff(void) { return s_diffTime; } // 1000 == 1 second (scaled by speed)
    inline static float GetTimeDiffF(void) { return s_fracTime; } // 1.0f == 1 second (scaled by speed)
    inline static float GetTimeDiffReal(void) { return s_diffTimeReal; } // 1000 = 1 second (real)
    inline static uint32 GetTicks(void) { return SDL_GetTicks() - s_ignoredTicks; }
    inline static void SetFixedDT(float dt) { s_fixedDT = dt; }
    static void ResetTime(void);
    static bool IsKeyPressed(SDLKey k);

    virtual void InitScreen(uint32 sizex, uint32 sizey, uint8 bpp = 0, uint32 extraflags = 0, bool full = false);
    virtual bool Setup(void);
    virtual void Shutdown(void);
    virtual const char *GetName(void) { return "EngineBase"; } // must be overloaded
    virtual void Run(float time = -1);

    void SetTitle(const char *title);
    inline void SetReset(bool r = true) { _reset = r; }
    inline bool IsReset(void) { return _reset; }
    //inline uint32 GetResX(void) { return _screen ? _screen->w : 0; }
    //inline uint32 GetResY(void) { return _screen ? _screen->h : 0; }
    //inline uint8 GetBPP(void) { return _screen ? _screen->format->BitsPerPixel : 0; }
    //inline const Camera& GetCamera(void) const { return _cameraPos; }
    //inline Camera *GetCameraPtr(void) { return &_cameraPos; }
    inline uint32 GetFPS(void) { return _fps; }
    //inline int32 GetMouseX(void) { return _mouseX + _cameraPos.x; }
    //inline int32 GetMouseY(void) { return _mouseY + _cameraPos.y; }
    inline void SetSleepTime(uint32 t) { _sleeptime = t; }

    /*inline void ToggleFullscreen(void)
    {
        SetFullscreen(!IsFullscreen());
    }
    void SetFullscreen(bool b);
    inline bool IsFullscreen(void) { return GetSurface()->flags & SDL_FULLSCREEN; }
    void SetResizable(bool b);
    inline bool IsResizable(void) { return GetSurface()->flags & SDL_RESIZABLE; }*/

    inline void TogglePause(void) { _paused = !_paused; }
    inline bool IsPaused(void) { return _paused; }
    inline void SetPaused(bool b) { _paused = b; }

    inline bool FrameLimit(void) { return _fpsMax && _fpsMin <= _fpsMax; }
    inline void FrameLimitMin(uint32 fps) { _fpsMin = fps; }
    inline void FrameLimitMax(uint32 fps) { _fpsMax = fps; }

    MouseState mouse;

    Camera *camera;
    float virtualOffX;
    float virtualOffY;
    
    ObsGrid obsgrid;
    SoundCore *sound;
    ObjectMgr *objmgr;
    RenderLayerMgr *layers;

    Texture *GetTexture(const char *name);
    inline Renderer *GetRenderer() const { return render; }

    virtual void UnregisterObject(ScriptObject *obj);
    virtual void ClearGarbage(bool deep);

    bool IsMouseButton(unsigned int btn);
    void SetMousePos(const Vector& pos);

    Vector ToWorldPosition(const Vector& v) const; // window -> world
    Vector ToWindowPosition(const Vector& v) const; // world -> window

    Vector ToWorldScale(const Vector& v) const; // window -> world
    Vector ToWindowScale(const Vector& v) const; // world -> window

    void CalcRenderLimits(unsigned int maxdim, float tileSize, int &x, int& y, int& x2, int& y2) const;

    Vector GetCameraPositionFor(const Vector &pos);

    void UpdateCullData();

    inline bool IsPause(int8 level = 0) const { return level < _pause; }
    inline void SetPause(int8 level) { _pause = level; }
    inline int8 GetPause() const { return _pause; }

    inline unsigned int GetRecursionDepth() const { return _recursionDepth; }


protected:

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
    virtual void OnLog(const char *, int color, int level, void *user);
    static void s_OnLog(const char *, int color, int level, void *user);

    void _ProcessEvents(void);
    void _CalcFPS(void);
    void _Render(void);
    void _Process(void);
    void _Reset(void);
    void _Idle(uint32 ms);

    std::string _wintitle;
    //uint32 _screenFlags; // stores surface flags set on screen creation
    
    //SDL_Rect _visibleBlockRect;
    uint32 _fps;
    uint32 _framecounter;
    uint32 _fpsMin, _fpsMax; // lower/upper bound of frame limiter, it will try to keep the FPS in between
    uint32 _fpsclock;
    uint32 _sleeptime;
    bool _paused;
    bool _reset;
    uint32 _quitRecursion;
    int32 _recursionDepth;
    int8 _pause;

    static std::vector<SDL_Joystick*> s_joysticks;

    // Dynamic time step
    static uint32 s_curFrameTime; // game time (scaled by speed)
    static uint32 s_lastFrameTimeReal; // last frame's SDL_GetTicks() -- not scaled by speed!
    static InterpolatedVector s_speed; // speed multiplicator, 1.0 = normal speed. should NOT be negative!
    static float s_accuTime; // accumulated time, for cases when s_speed is very small and the resulting frame time less then 1 ms
    static uint32 s_diffTime; // time diff per tick [uint32(s_fracTime)]
    static uint32 s_diffTimeReal; // time diff per tick, real time (not scaled by s_speed)
    static float s_fracTime; // (_diffTime * _speed) / 1000.0f

    // Fixed time step
    static float s_fixedAccu;
    static float s_fixedDT; // target dt for fixed timestep
    
    Renderer *render;

private:
    static void _InitJoystick(void); // this does nothing if joystick support was not explicitly initialized in SDL_Init()
    static bool s_quit;
    static uint32 s_ignoredTicks; // this value is subtracted from SDL_GetTicks(), to make the time start at 0 for every new EngineBase state. updated on EngineBase::_Reset().
};

extern EngineBase *engine;


#endif

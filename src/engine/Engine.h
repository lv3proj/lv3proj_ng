#ifndef ENGINE_H
#define ENGINE_H

#include "common.h"
#include <SDL/SDL.h>
#include "Vector.h"
#include "Camera.h"

class Renderer;
class SoundCore;
class Texture;

class EngineBase
{
public:
    EngineBase();
    virtual ~EngineBase();

    inline static bool IsQuit(void) { return s_quit; }
    inline static void SetQuit(bool q = true) { s_quit = q; }
    inline static SDL_Joystick *GetJoystick(uint32 i) { return i < s_joysticks.size() ? s_joysticks[i] : NULL; }
    inline static uint32 GetJoystickCount(void) { return s_joysticks.size(); }
    inline static void SetSpeed(float s) { s_speed = s; }
    inline static float GetSpeed(void) { return s_speed; }
    inline static uint32 GetCurFrameTime(void) { return s_curFrameTime; }
    inline static double GetCurFrameTimeF(void) { return (s_curFrameTime + (double)s_accuTime) / 1000.0; } // float is not precise enough, as this can get very high
    inline static uint32 GetTimeDiff(void) { return s_diffTime; } // 1000 == 1 second (scaled by speed)
    inline static float GetTimeDiffF(void) { return s_fracTime; } // 1.0f == 1 second (scaled by speed)
    inline static float GetTimeDiffReal(void) { return s_diffTimeReal; } // 1000 = 1 second (real)
    inline static uint32 GetTicks(void) { return SDL_GetTicks() - s_ignoredTicks; }
    static void ResetTime(void);
    static bool IsKeyPressed(SDLKey k);

    virtual void InitScreen(uint32 sizex, uint32 sizey, uint8 bpp = 0, uint32 extraflags = 0);
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

    Vector mouse;
    //Vector globalScale;
    Camera camera;
    float virtualOffX;
    float virtualOffY;

    SoundCore *sound;

    Texture *GetTexture(const char *name);
    inline Renderer *GetRenderer() const { return render; }

protected:

    virtual bool OnInit();
    virtual void OnReset();
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
    int32 _recursionDepth;

    static std::vector<SDL_Joystick*> s_joysticks;
    static uint32 s_curFrameTime; // game time (scaled by speed)
    static uint32 s_lastFrameTimeReal; // last frame's SDL_GetTicks() -- not scaled by speed!
    static float s_speed; // speed multiplicator, 1.0 = normal speed. should NOT be negative!
    static float s_accuTime; // accumulated time, for cases when s_speed is very small and the resulting frame time less then 1 ms
    static uint32 s_diffTime; // time diff per tick [uint32(s_fracTime)]
    static uint32 s_diffTimeReal; // time diff per tick, real time (not scaled by s_speed)
    static float s_fracTime; // (_diffTime * _speed) / 1000.0f
    
    Renderer *render;

private:
    static void _InitJoystick(void); // this does nothing if joystick support was not explicitly initialized in SDL_Init()
    static bool s_quit;
    static uint32 s_ignoredTicks; // this value is subtracted from SDL_GetTicks(), to make the time start at 0 for every new EngineBase state. updated on EngineBase::_Reset().
};

extern EngineBase *engine;


#endif

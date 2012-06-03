#include "Engine.h"
#include "Renderer.h"

// global, static
EngineBase *engine = NULL;


// see EngineBase.h for comments about these
uint32 EngineBase::s_curFrameTime;
uint32 EngineBase::s_lastFrameTimeReal;
bool EngineBase::s_quit;
float EngineBase::s_speed;
float EngineBase::s_accuTime;
uint32 EngineBase::s_diffTime;
uint32 EngineBase::s_diffTimeReal;
float EngineBase::s_fracTime;
uint32 EngineBase::s_ignoredTicks;
std::vector<SDL_Joystick*> EngineBase::s_joysticks;

EngineBase::EngineBase()
: _fps(0), _sleeptime(0), _framecounter(0), _paused(false),
_reset(false),
_fpsMin(60), _fpsMax(70),
virtualOffX(0),
virtualOffY(0),
_recursionDepth(-1),
render(NULL)
{
    log("Game Engine start.");
    engine = this;

    // static members
    s_curFrameTime = 0;
    s_quit = false;
    s_speed = 1.0;
    s_accuTime = 0;

    //sndCore.Init();

    _fpsclock = s_lastFrameTimeReal  = s_ignoredTicks = SDL_GetTicks();
    
    _InitJoystick();

    //_resPoolTimer.SetInterval(5000);

}

EngineBase::~EngineBase()
{
    engine = NULL;
    delete render;
}

void EngineBase::s_OnLog(const char *s, int color, int level, void *user)
{
    if(engine)
        engine->OnLog(s, color, level, user);
}

bool EngineBase::Setup(void)
{
    render = new Renderer();
    if(!render->Init())
        return false;
    log_setcallback(&s_OnLog, true, NULL); // TODO: param? newline?

    if(!OnInit())
        return false;

    return true;
}

void EngineBase::Shutdown(void)
{
    render->Shutdown();
    engine = NULL;
    // this should not be called from inside EngineBase::Run()

    //sndCore.StopMusic();
    /*delete objmgr;
    delete physmgr;
    delete _layermgr;
    resMgr.pool.Cleanup(true); // force deletion of everything
    resMgr.DropUnused(); // at this point, all resources should have a refcount of 0, so this removes all.
    sndCore.Destroy(); // must be deleted after all sounds were dropped by the ResourceMgr
    if(_screen)
        SDL_FreeSurface(_screen);*/
    
    for(uint32 i = 0; i < s_joysticks.size(); ++i)
        if(s_joysticks[i] && SDL_JoystickOpened(SDL_JoystickIndex(s_joysticks[i]))) // this is maybe a bit overcomplicated, but safe at least
            SDL_JoystickClose(s_joysticks[i]);

    // be sure we did a clean shutdown
    //resMgr.DbgCheckEmpty();
}

void EngineBase::SetTitle(const char *title)
{
    _wintitle = title;
}

void EngineBase::InitScreen(uint32 sizex, uint32 sizey, uint8 bpp /* = 0 */, uint32 extraflags /* = 0 */)
{
    render->SetInternalResolution(800, 600);
    render->SetScreenResolution(sizex, sizey);
    render->SetBPP(bpp ? bpp : 32);
    render->SetFullscreen(false);
    render->SetVsync(false);
    render->ApplySettings();

    /*
    if(sizex == GetResX() && sizey == GetResY() && (!bpp || bpp == GetBPP()) && ((_screen->flags | extraflags) == _screen->flags))
        return; // no change, nothing to do

    if(_screen)
        SDL_FreeSurface(_screen);
    _screenFlags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT | SDL_HWACCEL | extraflags;
    _screen = SDL_SetVideoMode(sizex, sizey, bpp, _screenFlags);
    _screenFlags &= ~SDL_FULLSCREEN; // this depends on current setting and should not be stored

    _gcnGfx->setTarget(GetSurface());
    */
}

void EngineBase::_InitJoystick(void)
{
    uint32 num = SDL_NumJoysticks();
    s_joysticks.resize(num);
    SDL_Joystick *jst = NULL;
    if(num)
    {
        logdetail("Found %u joysticks, enumerating...", num);
        SDL_JoystickEventState(SDL_ENABLE);
        for(uint32 i = 0; i < num; ++i)
        {
            jst = SDL_JoystickOpen(i); // TODO: do opened joysticks have to be closed explicitly? my guess is that SDL does it during cleanup/shutdown...
            if(jst)
            {
                log("Found joystick #%u (%s) with %u axes, %u buttons, %u hats, %u balls", i, SDL_JoystickName(i),
                    SDL_JoystickNumAxes(jst), SDL_JoystickNumButtons(jst), SDL_JoystickNumHats(jst), SDL_JoystickNumBalls(jst));
                SDL_JoystickEventState(SDL_ENABLE); // there is at least 1 active joystick, activate event mode
                
            }
            else
            {
                logerror("Found joystick #%u (%s), but failed to initialize!", i, SDL_JoystickName(i));
            }
            s_joysticks[i] = jst;
        }
    }
    else
    {
        logdetail("No joysticks found.");
    }
}

void EngineBase::Run(float runtime /* = -1 */)
{
    ++_recursionDepth;
    uint32 ms;
    uint32 diff;
    float diff_scaled;
    while(!s_quit)
    {
        if(IsReset())
            _Reset();

        ms = GetTicks();
        diff = ms - s_lastFrameTimeReal;
        if(diff > 127) // 127 ms max. allowed diff time
            diff = 127;
        diff_scaled = diff * s_speed;
        s_fracTime = diff_scaled / 1000.0f;

        s_accuTime += diff_scaled;
        s_diffTime = uint32(s_accuTime);
        s_accuTime -= s_diffTime; // s_accuTime stores the remaining sub-milliseconds
        s_diffTimeReal = diff; // limiting this to 127 max is intentional (its not the real REAL diff then, but oh well)
        
        _ProcessEvents();
        if(!_paused)
        {
            s_curFrameTime += s_diffTime;
            _Process();
            _Render();
        }

        _CalcFPS();
        s_lastFrameTimeReal = ms;

        if(runtime >= 0)
        {
            runtime = runtime - s_fracTime;
            if(runtime <= 0)
                break;
        }
    }
    --_recursionDepth;
}

void EngineBase::_ProcessEvents(void)
{
    SDL_Event evt;
    while(!s_quit && SDL_PollEvent(&evt))
    {
        if(!OnRawEvent(evt))
            continue;

        switch(evt.type)
        {
            case SDL_KEYDOWN:
                OnKeyDown(evt.key.keysym.sym, evt.key.keysym.mod);
                break;

            case SDL_KEYUP:
                OnKeyUp(evt.key.keysym.sym, evt.key.keysym.mod);
                break;

            case SDL_JOYAXISMOTION:
                OnJoystickEvent(evt.jaxis.type, evt.jaxis.which, evt.jaxis.axis, evt.jaxis.value);
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                OnJoystickEvent(evt.jbutton.type, evt.jbutton.which, evt.jbutton.button, evt.jbutton.state);
                break;

            case SDL_JOYHATMOTION:
                OnJoystickEvent(evt.jhat.type, evt.jhat.which, evt.jhat.hat, evt.jhat.value);
                break;

            case SDL_ACTIVEEVENT:
                OnWindowEvent(evt.active.gain);
                break;

            case SDL_VIDEORESIZE:
                OnWindowResize(evt.resize.w, evt.resize.h);
                break;

            case SDL_MOUSEMOTION:
                OnMouseEvent(evt.type, 0, evt.motion.state, evt.motion.x, evt.motion.y, evt.motion.xrel, evt.motion.yrel);
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                OnMouseEvent(evt.type, evt.button.button, evt.button.state, evt.button.x, evt.button.y, 0, 0);
                break;

            case SDL_QUIT:
                SetQuit(true);
                break;
        }
    }
}

void EngineBase::_CalcFPS(void)
{
    ++_framecounter;
    uint32 ms = SDL_GetTicks();
    if(ms - _fpsclock >= 1000 >> 2)
    {
        char buf[100];
        _fpsclock = ms;
        _fps = _framecounter << 2;
        _framecounter = 0;
        sprintf(buf, "%s - %u FPS - %u sleep [%u .. %u]%s", _wintitle.c_str(), _fps, _sleeptime, _fpsMin, _fpsMax, FrameLimit() ? "" : " (no limit)");
        SDL_WM_SetCaption((const char*)buf, NULL);
        if(FrameLimit())
        {
            if(_fps > _fpsMax)
            {
                ++_sleeptime;
            }
            else if(_sleeptime && _fps < _fpsMin)
            {
                --_sleeptime;
            }
        }
        else
            _sleeptime = 0;
    }
    if(FrameLimit())
        _Idle(_sleeptime);
}

void EngineBase::_Idle(uint32 ms)
{
    SDL_Delay(_sleeptime);
}

void EngineBase::_Process(void)
{
    float dt = GetTimeDiffF();
    //_layermgr->Update(GetCurFrameTime());
    //objmgr->Update(, GetCurFrameTime());

    //_resPoolTimer.Update(s_diffTimeReal);

    /*if(_resPoolTimer.Passed())
    {
        _resPoolTimer.Reset();
        resMgr.pool.Cleanup();
    }*/

    camera.update(dt);

    OnUpdate(dt);
}

bool EngineBase::OnInit()
{
    return true;
}


// Handle a raw SDL_Event before anything else. return true for further processing,
// false to drop the event and NOT pass it to other On..Event functions
bool EngineBase::OnRawEvent(SDL_Event& evt)
{
    return true;
}

void EngineBase::OnWindowEvent(bool active)
{
    // TODO: pause if lost focus?
}

void EngineBase::OnMouseEvent(uint32 type, uint32 button, uint32 state, uint32 x, uint32 y, int32 rx, int32 ry)
{
    mouse.x = x;
    mouse.y = y;
}

void EngineBase::OnJoystickEvent(uint32 type, uint32 device, uint32 id, int32 val)
{
}

void EngineBase::OnKeyDown(SDLKey key, SDLMod mod)
{
    if(mod & KMOD_LALT)
    {
        if(key == SDLK_F4)
            SetQuit(true);
        //else if(key == SDLK_RETURN)
        //    ToggleFullscreen();
    }

    if(key == SDLK_PAUSE)
    {
        TogglePause();
    }
}

void EngineBase::OnKeyUp(SDLKey key, SDLMod mod)
{
}

void EngineBase::OnWindowResize(uint32 newx, uint32 newy)
{
    InitScreen(newx, newy);
}

void EngineBase::OnLog(const char *, int color, int level, void *user)
{
}

#include "TestRenderObject.h"

static TestRenderObject ro;

void EngineBase::OnUpdate(float dt)
{
    ro.update(dt);
    //logdev("%f  %f", ro.position.x, ro.position.y);
}

void EngineBase::OnReset()
{
}

bool EngineBase::IsKeyPressed(SDLKey k)
{
    int numkeys = 0;
    uint8 *state = SDL_GetKeyState(&numkeys);
    return k < numkeys && state[k];
}

void EngineBase::_Render(void)
{
    render->clear();
    render->setupRenderPositionAndScale();
    render->renderObject(&ro);

    render->show();
}


void EngineBase::_Reset(void)
{
    logdetail("EngineBase: Reset!");
    OnReset();
    _reset = false;
    //objmgr->RemoveAll();
    //_layermgr->Clear();
    //physmgr->SetDefaults();
    //resMgr.pool.Cleanup();
    //resMgr.DropUnused();
    //resMgr.vfs.Prepare(true);
    //resMgr.vfs.Reload(true);
    ResetTime();
}

void EngineBase::ResetTime(void)
{
    s_ignoredTicks = SDL_GetTicks();
    s_accuTime = 0;
    s_curFrameTime = 0;
}
/*
void EngineBase::SetFullscreen(bool b)
{
    if(b == IsFullscreen())
        return; // no change required

    uint32 flags = _screen->flags | _screenFlags;

    // toggle between fullscreen, preserving other flags
    if(b)
        flags |= SDL_FULLSCREEN; // add fullscreen flag
    else
        flags &= ~SDL_FULLSCREEN; // remove fullscreen flag

    InitScreen(GetResX(), GetResY(), GetBPP(), flags);
}

void EngineBase::SetResizable(bool b)
{
    if(b == IsResizable())
        return; // no change required

    uint32 flags = _screen->flags | _screenFlags;

    // toggle between fullscreen, preserving other flags
    if(b)
        flags |= SDL_RESIZABLE; // add resizable flag
    else
        flags &= ~SDL_RESIZABLE; // remove resizable flag

    InitScreen(GetResX(), GetResY(), GetBPP(), flags);
}
*/




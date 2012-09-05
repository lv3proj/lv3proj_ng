#include "Engine.h"
#include "Renderer.h"
#include "SoundCore.h"
#include "ResourceMgr.h"
#include "SDLSurfaceResource.h"
#include "Quad.h"
#include "Texture.h"
#include "AnimatedTexture.h"
#include "ObjectMgr.h"
#include "Camera.h"
#include "RenderLayerMgr.h"


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
_reset(false), _quitRecursion(0),
_fpsMin(60), _fpsMax(70),
virtualOffX(0),
virtualOffY(0),
_recursionDepth(-1),
render(NULL),
sound(NULL),
layers(NULL),
camera(NULL),
_pause(0)
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
    delete layers;
    delete camera;
    delete objmgr;
    delete render;
    delete sound;

    engine = NULL;
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
    sound = new SoundCore();
    if(!sound->Init())
        return false;
    log_setcallback(&s_OnLog, true, NULL); // TODO: param? newline?

    objmgr = new ObjectMgr();
    layers = new RenderLayerMgr();
    camera = new Camera();


    if(!OnInit())
        return false;

    return true;
}

// this should not be called from inside EngineBase::Run()
void EngineBase::Shutdown(void)
{
    obsgrid.Clear();
    layers->ClearAll();
    objmgr->ClearAll();
    sound->Shutdown();
    ClearGarbage(true);
    render->Shutdown();
    resMgr.Shutdown();

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

    const ResourceStore& rs = resMgr.GetResourcesOfType(RESOURCE_TEXTURE);
    logdetail("Reloading %u textures...", (unsigned int)rs.size());
    for(ResourceStore::const_iterator it = rs.begin(); it != rs.end(); ++it)
    {
        logdev("Reloading texture: %s", it->second->name());
        ((Texture*)(it->second))->reload();
    }

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
        if(_quitRecursion)
        {
            --_quitRecursion;
            break;
        }

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
    mouse.worldRel = mouse.winRel = Vector(0, 0);
    mouse.wheelRel = 0;

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

    camera->update(dt);

    objmgr->Update(dt);

    resMgr.Update(dt);

    UpdateCullData();

    OnUpdate(dt);

    ClearGarbage(false);
}

void EngineBase::UpdateCullData()
{
    RenderObject::s_cullX1 = camera->position.x - (virtualOffX * camera->invScale.x);
    RenderObject::s_cullY1 = camera->position.y - (virtualOffY * camera->invScale.y);
    RenderObject::s_cullX2 = camera->position.x + ((virtualOffX+800) * camera->invScale.x);
    RenderObject::s_cullY2 = camera->position.y + ((virtualOffY+600) * camera->invScale.y);
}

bool EngineBase::IsMouseButton(unsigned int btn)
{
    return mouse.buttons & SDL_BUTTON(btn);
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
    const RenderSettings &rr = render->getSettings();
    Vector lastMouseWinPos = mouse.winPos;

    mouse.winPos.x = rangeTransform<float>(x, 0, rr.pixelsW, -virtualOffX, rr.virtualW + virtualOffX);
    mouse.winPos.y = rangeTransform<float>(y, 0, rr.pixelsH, -virtualOffY, rr.virtualH + virtualOffY);

    mouse.winRel = mouse.winPos - lastMouseWinPos;

    mouse.worldPos = ToWorldPosition(mouse.winPos);
    mouse.worldRel = ToWorldScale(mouse.winRel);

    if(state)
    {
        mouse.buttons |= SDL_BUTTON(button);

        // This is reset at the beginning of input handling.
        // SDL always sends 2 events for wheel movement, one with state 1, and the next one with state 0.
        // We only handle one of them.
        switch(button)
        {
            case SDL_BUTTON_WHEELUP:
                ++mouse.wheelRel;
                break;

            case SDL_BUTTON_WHEELDOWN:
                --mouse.wheelRel;
                break;
        }
    }
    else
    {
        mouse.buttons &= ~SDL_BUTTON(button);
    }
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

void EngineBase::OnUpdate(float dt)
{
}

void EngineBase::OnReset()
{
    _pause = 0;
}

void EngineBase::OnRender()
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
    camera->update(0);

    render->clear();
    //render->setupRenderPositionAndScale();

    layers->Render();

    OnRender();

    render->show();
}


void EngineBase::_Reset(void)
{
    if(_recursionDepth)
    {
        _quitRecursion = _recursionDepth;
        return;
    }

    logdetail("EngineBase: Reset!");
    _reset = false;
    layers->ClearAll();
    objmgr->ClearAll();
    OnReset();
    ClearGarbage(true);
    ResetTime();
}

void EngineBase::ResetTime(void)
{
    s_ignoredTicks = SDL_GetTicks();
    s_accuTime = 0;
    s_curFrameTime = 0;
}

Texture *EngineBase::GetTexture(const char *name)
{
    Texture *tex = resMgr._GetTexture(name);
    if(!tex)
    {
        if(!*name)
        {
            tex = render->createNullTexture();
        }
        else if(fileHasExtension(name, "anim"))
        {
            Anim *ani = resMgr.LoadAnim(name);
            if(ani)
            {
                tex = new AnimatedTexture(ani);
                ani->decref();
            }
        }
        else
            tex = render->createTexture(name);

        if(tex)
            resMgr.Add(tex);
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
        sound->ClearGarbage();
        resMgr.DropUnused();
    }
}

Vector EngineBase::ToWorldPosition(const Vector& v) const
{
    const RenderSettings& rr = render->getSettings();
    Vector ret = v;
    ret.x -= (rr.virtualW / 2.0f);
    ret.y -= (rr.virtualH / 2.0f);
    ret *= camera->invScale;
    ret += camera->screenCenter;
    return ret;
}

Vector EngineBase::ToWindowPosition(const Vector& v) const
{
    const RenderSettings& rr = render->getSettings();
    Vector ret = v;
    ret -= camera->screenCenter;
    ret *= camera->scale;
    ret.x += (rr.virtualW / 2.0f);
    ret.y += (rr.virtualH / 2.0f);
    return ret;
}

Vector EngineBase::ToWorldScale(const Vector& v) const
{
    return v * camera->scale;
}

Vector EngineBase::ToWindowScale(const Vector& v) const
{
    return v * camera->invScale;
}

void EngineBase::CalcRenderLimits(unsigned int maxdim, float tileSize, int &x, int& y, int& x2, int& y2) const
{
    const RenderSettings& rr = render->getSettings();

    // Find out max. visible screen coordinates
    Vector upperLeftTile  = ToWorldPosition(Vector(-virtualOffX, -virtualOffY));
    Vector lowerRightTile = ToWorldPosition(Vector(rr.virtualW + virtualOffX, rr.virtualH + virtualOffY));

    // Transform these into tile coords
    upperLeftTile /= tileSize;
    lowerRightTile /= tileSize;

    // Make sure the right- and bottom-most tiles are on the screen too
    lowerRightTile.x += 2;
    lowerRightTile.y += 2;

    // Prevent the view going off bounds
    x = (upperLeftTile.x < 0 ? 0 : int(upperLeftTile.x));
    y = (upperLeftTile.y < 0 ? 0 : int(upperLeftTile.y));
    x2 = (lowerRightTile.x > maxdim ? maxdim : int(lowerRightTile.x));
    y2 = (lowerRightTile.y > maxdim ? maxdim : int(lowerRightTile.y));
}


void EngineBase::SetMousePos(const Vector& pos)
{
    const RenderSettings &rr = render->getSettings();
    Vector newpos;
    mouse.winPos = pos;
    mouse.worldPos = ToWorldPosition(pos);
    newpos.x = rangeTransform<float>(pos.x, -virtualOffX, rr.virtualW + virtualOffX, 0, rr.pixelsW);
    newpos.y = rangeTransform<float>(pos.y, -virtualOffY, rr.virtualH + virtualOffY, 0, rr.pixelsH);

    //logdev("Warp mouse (%.3f, %.3f)", pos.x, pos.y);
    SDL_WarpMouse(newpos.x, newpos.y);
}

Vector EngineBase::GetCameraPositionFor(const Vector &pos)
{
    Vector dest = pos;
    dest.x += (-400 * camera->invScale.x);
    dest.y += (-300 * camera->invScale.y);
    return dest;
}

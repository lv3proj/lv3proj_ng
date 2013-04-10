#include "LuaEngine.h"
#include "LuaDefines.h"
#include "LuaInterface.h"
#include "ScriptedEngine.h"
#include "Quad.h"
#include "MemResource.h"
#include "ResourceMgr.h"
#include "ObjectMgr.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"
#include "Camera.h"
#include "Renderer.h"
#include "SoundCore.h"
#include "PlatformSpecific.h"
#include "ScriptedEntity.h"
#include "Texture.h"
#include "TileGrid.h"
#include "collision/AABB.h"
#include "collision/Circle.h"
#include "collision/Line.h"
#include "Arenas.h"

struct LuaFunctions
{
    const char *name;
    lua_CFunction func;
};

static int loadFile_helper(lua_State *L, const char *fn)
{
    MemResource *data = resMgr.LoadFile(fn);
    if(!data)
    {
        lua_pushfstring(L, "cannot open %s", fn);
        return LUA_ERRFILE;
    }

    int result = luaL_loadbuffer(L, (const char*)data->ptr(), data->size(), fn);
    data->decref();
    return result;
}


luaFunc(dofile_wrap)
{
    std::string fn = "scripts/";
    fn += getCStrSafe(L);

    int n = lua_gettop(L);
    if (loadFile_helper(L, fn.c_str()) != 0)
        lua_error(L);
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L) - n;
}

luaFunc(loadfile_wrap)
{
    std::string fn = "scripts/";
    fn += getCStrSafe(L);

    if (loadFile_helper(L, fn.c_str()) == 0)  /* OK? */
        return 1;
    else
    {
        lua_pushnil(L);
        lua_insert(L, -2);  /* put before error message */
        return 2;  /* return nil plus error message */
    }
}

static RenderLayer *getLayerByID(lua_State *L, int idx = 1)
{
    if(lua_isnumber(L, idx))
        return engine->layers->GetLayer(lua_tointeger(L, idx));
    else if(lua_isstring(L, idx))
        return engine->layers->GetLayer(getCStr(L, idx));
    else if(lua_isnil(L, idx))
        return engine->layers->GetLayer(0);
    return NULL;
}

luaFunc(wait)
{
    engine->Run(lua_tonumber(L, 1));
    luaReturnNil();
}

luaFunc(isMouseButton)
{
    luaReturnBool(engine->IsMouseButton(lua_tointeger(L, 1)));
}

luaFunc(getMouseWindowPos)
{
    luaReturnVec2(engine->mouse.winPos.x, engine->mouse.winPos.y);
}

luaFunc(getMouseWindowRel)
{
    luaReturnVec2(engine->mouse.winRel.x, engine->mouse.winRel.y);
}

luaFunc(getMouseWorldRel)
{
    luaReturnVec2(engine->mouse.worldRel.x, engine->mouse.worldRel.y);
}

luaFunc(getMouseWorldPos)
{
    luaReturnVec2(engine->mouse.worldPos.x, engine->mouse.worldPos.y);
}

luaFunc(setMousePos)
{
    Vector v(lua_tonumber(L, 1), lua_tonumber(L, 2));
    engine->SetMousePos(v);
    luaReturnNil();
}


luaFunc(drawLine)
{
    float x1 = lua_tonumber(L, 1);
    float y1 = lua_tonumber(L, 2);
    float x2 = lua_tonumber(L, 3);
    float y2 = lua_tonumber(L, 4);
    float lw = lua_tonumber(L, 5);
    float r = lua_tonumber(L, 6);
    float g = lua_tonumber(L, 7);
    float b = lua_tonumber(L, 8);
    float a = lua_tonumber(L, 9);

    engine->GetRenderer()->drawLine(x1, y1, x2, y2, lw, r, g, b, a);

    luaReturnNil();
}

luaFunc(getMouseWheelRel)
{
    luaReturnInt(engine->mouse.wheelRel);
}

luaFunc(msgbox)
{
    MsgBox(getCStrSafe(L));
    luaReturnNil();
}

luaFunc(clearGarbage)
{
    engine->ClearGarbage(true);
    luaReturnNil();
}

luaFunc(setTile)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("setTile(): Invalid layer");
        luaReturnNil();
    }
    unsigned int x = lua_tointeger(L, 2);
    unsigned int y = lua_tointeger(L, 3);
    layer->tiles->SetTileByName(x, y, getCStr(L, 4)); // can be NULL
    engine->obsgrid.UpdateTile(x, y);
    luaReturnNil();
}

luaFunc(getTile)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("getTile(): Invalid layer");
        luaReturnNil();
    }
    unsigned int x = lua_tointeger(L, 2);
    unsigned int y = lua_tointeger(L, 3);
    Tile *tile = layer->tiles->GetTileSafe(x, y);
    if(tile && tile->getTexture())
        luaReturnStr(tile->getTexture()->name());

    luaReturnNil();
}

luaFunc(clearTiles)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("clearTiles(): Invalid layer");
        luaReturnNil();
    }

    layer->tiles->Clear();
    luaReturnNil();
}

luaFunc(setTileGridSize)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("setTileGridSize(): Invalid layer");
        luaReturnNil();
    }
    layer->tiles->SetSize(lua_tointeger(L, 2));
    luaReturnNil();
}

luaFunc(setTileGridCollision)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("setTileGridSize(): Invalid layer");
        luaReturnNil();
    }
    layer->tiles->colliding = getBool(L, 2);
    luaReturnNil();
}

luaFunc(initObsGrid)
{
    engine->obsgrid.Init(lua_tointeger(L, 1), lua_tointeger(L, 2));
    engine->obsgrid.Setup();
    luaReturnNil();
}

luaFunc(setLayerParallax)
{
    RenderLayer *layer = getLayerByID(L, 1);
    if(!layer)
    {
        logerror("setLayerParallax(): Invalid layer");
        luaReturnNil();
    }
    layer->parallax = Vector(lua_tonumber(L, 2), lua_tonumber(L, 3));
    luaReturnNil();
}

luaFunc(setPause)
{
    engine->SetPause(lua_tointeger(L, 1));
    luaReturnNil();
}

luaFunc(isPause)
{
    luaReturnBool(engine->IsPause(lua_tointeger(L, 1)));
}

luaFunc(getPause)
{
    luaReturnInt(engine->GetPause());
}

luaFunc(isKey)
{
    luaReturnBool(engine->IsKeyPressed((SDLKey)lua_tointeger(L, 1)));
}

luaFunc(getRecursionDepth)
{
    luaReturnInt(engine->GetRecursionDepth());
}

luaFunc(getVirtualOffs)
{
    luaReturnVec2(engine->virtualOffX, engine->virtualOffY);
}

luaFunc(worldToWindow)
{
    Vector t = engine->ToWorldPosition(Vector(lua_tonumber(L, 1), lua_tonumber(L, 2)));
    luaReturnVec2(t.x, t.y);
}

luaFunc(windowToWorld)
{
    Vector t = engine->ToWindowPosition(Vector(lua_tonumber(L, 1), lua_tonumber(L, 2)));
    luaReturnVec2(t.x, t.y);
}

luaFunc(getScreenCenter)
{
    const Vector& t = engine->camera->screenCenter;
    luaReturnVec2(t.x, t.y);
}

luaFunc(getWallNormal)
{
    unsigned int resolution = (unsigned int)lua_tointeger(L, 3);
    unsigned int skip = (unsigned int)lua_tointeger(L, 4);
    unsigned char bits = (unsigned char)lua_tointeger(L, 5);
    if(!resolution)
        resolution = 5;
    if(!skip)
        skip = 1;
    if(!bits)
        bits = ~0;

    Vector normal;
    engine->obsgrid.getNormal(Vector(lua_tonumber(L, 1), lua_tonumber(L, 2)), normal, resolution, skip, bits);
    luaReturnVec2(normal.x, normal.y);
}

luaFunc(setFixedDT)
{
    float dt = lua_tonumber(L, 1);
    if(dt > 0)
        engine->SetFixedDT(dt);
    luaReturnNil();
}

luaFunc(setSpeed)
{
    float curspeed = engine->GetSpeed();
    engine->SetSpeed(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tointeger(L, 3), lua_toboolean(L, 4), lua_toboolean(L, 5));
    luaReturnNum(curspeed);
}

luaFunc(isObjectValid)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, 1);
    luaReturnBool(su && su->obj);
}

luaFunc(getObs)
{
    luaReturnInt(engine->obsgrid.getObs(lua_tointeger(L, 1), lua_tointeger(L, 2)));
}

luaFunc(resetEngine)
{
    engine->SetReset(true);
    luaReturnNil();
}

static LuaFunctions s_functab[] =
{
    { "dofile", l_dofile_wrap },
    { "loadfile", l_loadfile_wrap },
    luaRegister(wait),
    luaRegister(drawLine),
    luaRegister(isMouseButton),
    luaRegister(getMouseWindowPos),
    luaRegister(getMouseWindowRel),
    luaRegister(getMouseWorldPos),
    luaRegister(getMouseWorldRel),
    luaRegister(getMouseWheelRel),
    luaRegister(setMousePos),
    luaRegister(msgbox),
    luaRegister(clearGarbage),
    luaRegister(setTile),
    luaRegister(getTile),
    luaRegister(clearTiles),
    luaRegister(setTileGridSize),
    luaRegister(setTileGridCollision),
    luaRegister(initObsGrid),
    luaRegister(setLayerParallax),
    luaRegister(isPause),
    luaRegister(setPause),
    luaRegister(getPause),
    luaRegister(isKey),
    luaRegister(getRecursionDepth),
    luaRegister(getVirtualOffs),
    luaRegister(windowToWorld),
    luaRegister(worldToWindow),
    luaRegister(getScreenCenter),
    luaRegister(getWallNormal),
    luaRegister(setFixedDT),
    luaRegister(setSpeed),
    luaRegister(isObjectValid),
    luaRegister(getObs),
    luaRegister(resetEngine),

    { NULL, NULL }
};


void lua_register_enginefuncs(lua_State *L)
{
    for(unsigned int i = 0; s_functab[i].name; ++i)
    {
        lua_register(L, s_functab[i].name, s_functab[i].func);
    }
}

static RenderObject *getRO(lua_State *L, int idx = 1)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, idx);
    if(su && ((su->type & OT_RENDEROBJECT) == OT_RENDEROBJECT))
        return (RenderObject*)su->obj;
    return NULL;
}

static Quad *getQuad(lua_State *L, int idx = 1)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, idx);
    if(su && ((su->type & OT_QUAD) == OT_QUAD))
        return (Quad*)su->obj;
    return NULL;
}

static SoundFile *getSound(lua_State *L, int idx = 1)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, idx);
    if(su && ((su->type & OT_SOUND) == OT_SOUND))
        return (SoundFile*)su->obj;
    return NULL;
}

static Entity *getEntity(lua_State *L, int idx = 1)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, idx);
    if(su && ((su->type & OT_ENTITY) == OT_ENTITY))
        return (Entity*)su->obj;
    return NULL;
}


static void doInterpolateVec1(InterpolatedVector& v, lua_State *L, int startidx)
{
    v.interpolateTo(lua_tonumber(L, startidx),        // value
                    lua_tonumber(L, startidx+1),      // time
                    lua_tointeger(L, startidx+2),     // loop
                    getBool(L, startidx+3),           // ping pong
                    getBool(L, startidx+4));          // ease
}

static void doInterpolateVec2(InterpolatedVector& v, lua_State *L, int startidx)
{
    v.interpolateTo(Vector(lua_tonumber(L, startidx), lua_tonumber(L, startidx+1)),
                    lua_tonumber(L, startidx+2),
                    lua_tointeger(L, startidx+3),
                    getBool(L, startidx+4),
                    getBool(L, startidx+5));
}

static void doInterpolateVec3(InterpolatedVector& v, lua_State *L, int startidx)
{
    v.interpolateTo(Vector(lua_tonumber(L, startidx), lua_tonumber(L, startidx+1), lua_tonumber(L, startidx+2)),
                    lua_tonumber(L, startidx+3),
                    lua_tointeger(L, startidx+4),
                    getBool(L, startidx+5),
                    getBool(L, startidx+6));
}

static inline int doPushVec1(const Vector& v, lua_State *L)
{
    lua_pushnumber(L, v.x);
    return 1;
}

static inline int doPushVec2(const Vector& v, lua_State *L)
{
    lua_pushnumber(L, v.x);
    lua_pushnumber(L, v.y);
    return 2;
}

static inline int doPushVec3(const Vector& v, lua_State *L)
{
    lua_pushnumber(L, v.x);
    lua_pushnumber(L, v.y);
    lua_pushnumber(L, v.z);
    return 3;
}

static inline int doPushVecNull1(lua_State *L)
{
    lua_pushnumber(L, 0);
    return 1;
}

static inline int doPushVecNull2(lua_State *L)
{
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    return 2;
}

static inline int doPushVecNull3(lua_State *L)
{
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    return 3;
}

// ScriptObjectUserStruct must be on top of stack; leaves it there.
static void registerScriptObjectStruct(lua_State *L, lua_CFunction gcfunc)
{
    // -- Register object in global registry table --
    // now [su]
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, -1);
    if(su->obj->isManaged())
    {
        registerUserdata(L, su->obj);
    }

    // -- Assign metatable -- (Will be populated in the scripts)
    lua_newtable(L);
    // now [su][t]
    if(gcfunc)
    {
        lua_pushliteral(L, "__gc");
        // now [su][t]["__gc"]
        lua_pushcfunction(L, gcfunc);
        // now [su][t]["__gc"][gcfunc]
        lua_settable(L, -3);
        // now [su][t]
    }
    lua_setmetatable(L, -2);
    // now [su]
}

static int registerObject(lua_State *L, ScriptObject *obj, ScriptObjectType type, lua_CFunction gcfunc)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_newuserdata(L, sizeof(ScriptObjectUserStruct));
    obj->scriptBindings = su;
    su->obj = obj;
    su->type = type;
    registerScriptObjectStruct(L, gcfunc);
    if(obj->isManaged())
        engine->objmgr->AddObject(obj);
    return 1;
}

luaFn(ro_delete)
{
    RenderObject *ro = getRO(L);
    if(ro)
        ro->kill(lua_tonumber(L, 2));
    luaReturnSelf();
}

luaFn(ro_blend)
{
    RenderObject *ro = getRO(L);
    if(ro)
        ro->setBlendType((BlendType)lua_tointeger(L, 2));
    luaReturnSelf();
}

luaFn(ro_addChild)
{
    RenderObject *ro = getRO(L);
    RenderObject *child = getRO(L, 2);
    if(ro && child)
        ro->addChild(child);
    luaReturnSelf();
}

luaFn(ro_getAbsolutePosition)
{
    RenderObject *ro = getRO(L);
    Vector ret;
    if(ro)
        ret = ro->getAbsolutePosition();
    luaReturnVec2(ret.x, ret.y);
}

luaFn(ro_addVel)
{
    RenderObject *ro = getRO(L);
    if(ro)
    {
        ro->velocity.x += lua_tonumber(L, 2);
        ro->velocity.y += lua_tonumber(L, 3);
    }
    luaReturnSelf();
}



// ----------- Evil define hackery -----------------
#define MAKE_RO_VEC_MTH(setter, getter, v, c) \
luaFn(ro_##setter) \
{ \
    RenderObject *ro = getRO(L); \
    if(ro) \
        doInterpolateVec##c(ro->v, L, 2); \
    luaReturnSelf(); \
} \
luaFn(ro_##getter) \
{ \
    RenderObject *ro = getRO(L); \
    if(ro) \
        return doPushVec##c(ro->v, L); \
    return doPushVecNull##c(L); \
} \

#define RO_SET_STUFF \
MAKE_RO_VEC_MTH(position,   getPosition,    position,   2) \
MAKE_RO_VEC_MTH(offset,     getOffset,      offset,     2) \
MAKE_RO_VEC_MTH(scale,      getScale,       scale,      2) \
MAKE_RO_VEC_MTH(scale2,     getScale2,      scale2,      2) \
MAKE_RO_VEC_MTH(color,      getColor,       color,      3) \
MAKE_RO_VEC_MTH(color2,     getColor2,      color2,     3) \
MAKE_RO_VEC_MTH(alpha,      getAlpha,       alpha,      1) \
MAKE_RO_VEC_MTH(alpha2,     getAlpha2,      alpha2,     1) \
MAKE_RO_VEC_MTH(rotate,     getRotation,    rotation,   1) \
MAKE_RO_VEC_MTH(rotate2,    getRotation2,   rotation2,  1) \
MAKE_RO_VEC_MTH(velocity,   getVelocity,    velocity,   2) \
MAKE_RO_VEC_MTH(gravity,    getGravity,     gravity,    2) \
MAKE_RO_VEC_MTH(friction,   getFriction,    friction,    2)

RO_SET_STUFF // Create the functions


luaFn(ro_isVisible)
{
    RenderObject *ro = getRO(L);
    luaReturnBool(ro ? ro->isVisible() : false);
}

luaFn(ro_parallax)
{
    RenderObject *ro = getRO(L);
    if(ro)
        ro->parallax = Vector(lua_tonumber(L, 2), lua_tonumber(L, 3));
    luaReturnSelf();
}

luaFn(ro_getLayer)
{
    RenderObject *ro = getRO(L);
    luaReturnInt((ro && ro->getLayerPtr()) ? ro->getLayerPtr()->GetID() : 0);
}

luaFn(ro_setPauseLevel)
{
    RenderObject *ro = getRO(L);
    if(ro)
        ro->setPauseLevel(lua_tointeger(L, 2));
    luaReturnSelf();
}

luaFn(ro_getParent)
{
    RenderObject *ro = getRO(L);
    if(ro && ro->getParent())
        luaReturnObject(ro->getParent()); // FIXME: if this does not have a script pointer, it will return nil. Could create one instead?
    luaReturnNil();
}

luaFn(ro_updatePhysics)
{
    RenderObject *ro = getRO(L);
    if(ro)
        ro->updatePhysics(lua_tonumber(L, 2));
    luaReturnSelf();
}

luaFn(ro_vectorTo)
{
    RenderObject *a = getRO(L);
    RenderObject *b = getRO(L, 2);
    if(a && b)
    {
        Vector av = a->position + a->offset;
        Vector bv = b->position + b->offset;
        Vector d = bv - av;
        luaReturnVec2(d.x, d.y);
    }
    luaReturnVec2(0, 0);
}

#undef MAKE_RO_VEC_MTH

#define MAKE_RO_VEC_MTH(setter, getter, v, c) \
{ #setter, ro_##setter }, \
{ #getter, ro_##getter },

//-------------------------------------------


luaFn(quad_new)
{
    RenderLayer *lr = getLayerByID(L, 2);
    if(!lr)
    {
        // TODO: nicer output, add stack
        logerror("ERROR: quad.new() invalid target layer");
        luaReturnNil();
    }

    Quad *q = Quad::create(getCStr(L, 1), lua_tointeger(L, 3), lua_tointeger(L, 4));
    lr->Add(q);
    return registerObject(L, q, OT_QUAD, NULL);
}

luaFn(quad_texture)
{
    Quad *q = getQuad(L);
    if(q)
        q->setTexture(getCStrSafe(L, 2));
    luaReturnSelf();
}

luaFn(quad_getTexture)
{
    Quad *q = getQuad(L);
    if(q && q->getTexture())
    {
        luaReturnStr(q->getTexture()->name());
    }
    luaReturnNil();
}

luaFn(quad_getWH)
{
    Quad *q = getQuad(L);
    if(q)
        luaReturnVec2(q->getWidth(), q->getHeight());

    luaReturnVec2(0, 0);
}

luaFn(quad_setWH)
{
    Quad *q = getQuad(L);
    if(q)
        q->setWH(lua_tonumber(L, 2), lua_tonumber(L, 3));

    luaReturnSelf();
}

luaFn(entity_new)
{
    Entity *e = ScriptedEntity::create(scriptedEngine->script);
    RenderLayer *lr = getLayerByID(L, 1);
    if(!lr || lr->GetID() == 0)
        lr = engine->layers->GetLayer(10); // FIXME: which layer is good?
    lr->Add(e);
    return registerObject(L, e, OT_ENTITY, NULL);
}

luaFn(entity_setAABBCollider)
{
    Entity *e = getEntity(L);
    if(e)
    {
        if(lua_gettop(L) > 1)
        {
            e->setCollider(new AABB(e,
                            Vector(lua_tonumber(L, 2), lua_tonumber(L, 3)),
                            Vector(lua_tonumber(L, 4), lua_tonumber(L, 5))));
        }
        else if(const Texture *tex = e->getTexture())
        {
            const float w2 = tex->getHalfWidth();
            const float h2 = tex->getHalfHeight();
            e->setCollider(new AABB(e, Vector(-w2, -h2), Vector(w2, h2)));
        }
        // TODO: else error?
    }
    luaReturnSelf();
}

luaFn(entity_setCircleCollider)
{
    Entity *e = getEntity(L);
    if(e)
        e->setCollider(new Circle(e, lua_tonumber(L, 2)));
    luaReturnSelf();
}

luaFn(entity_setLineCollider)
{
    Entity *e = getEntity(L);
    if(e)
        e->setCollider(new Line(e, Vector(lua_tonumber(L, 2), lua_tonumber(L, 3)), Vector(lua_tonumber(L, 4), lua_tonumber(L, 5))));
    luaReturnSelf();
}

luaFn(entity_collideWith)
{
    Entity *e = getEntity(L);
    if(e)
    {
        Vector result;
        if(e->collidesWith(getEntity(L, 2), &result))
        {
            lua_pushboolean(L, 1);
            lua_pushnumber(L, result.x);
            lua_pushnumber(L, result.y);
            return 3;
        }
    }

    luaReturnBool(false);
}

luaFn(entity_collideGrid)
{
    Entity *e = getEntity(L);
    if(e)
    {
        Vector result;
        if(scriptedEngine->obsgrid.collidesWith(e->getCollider(), &result))
        {
            lua_pushboolean(L, 1);
            lua_pushnumber(L, result.x);
            lua_pushnumber(L, result.y);
            return 3;
        }
    }

    luaReturnBool(false);
}

luaFn(_sound_gc)
{
    SoundFile *sound = getSound(L);
    if(sound)
    {
        sound->SetDeleteWhenStopped(true);
        engine->UnregisterObject(sound);
    }
    luaReturnNil();
}

luaFn(sound_new)
{
    const char *fn = getCStr(L);
    if(!fn)
        return 0;

    SoundFile *sf = engine->sound->GetSound(fn);
    if(!sf)
        luaReturnNil();

    return registerObject(L, sf, OT_SOUND, _sound_gc);
}

luaFn(sound_play)
{
    SoundFile *sf = getSound(L);
    if(sf)
    {
        int loops = lua_tointeger(L, 2);
        sf->Play(loops);
        luaReturnSelf();
    }
    luaReturnNil();
}

luaFn(sound_fadeOut)
{
    SoundFile *sf = getSound(L);
    if(sf)
    {
        sf->FadeOut(lua_tonumber(L, 2));
        luaReturnSelf();
    }
    luaReturnNil();
}

luaFn(sound_fadeIn)
{
    SoundFile *sf = getSound(L);
    if(sf)
    {
        float t = lua_tonumber(L, 2);
        int loops = lua_tointeger(L, 3);
        sf->FadeIn(t, loops);
        luaReturnSelf();
    }
    luaReturnNil();
}

luaFn(sound_stop)
{
    SoundFile *sf = getSound(L);
    if(sf)
    {
        sf->Stop();
        luaReturnSelf();
    }
    luaReturnNil();
}

luaFn(sound_volume)
{
    SoundFile *sf = getSound(L);
    if(sf)
    {
        float vol = lua_tonumber(L, 2);
        sf->SetVolume(vol);
        luaReturnSelf();
    }
    luaReturnNil();
}

luaFn(sound_getVolume)
{
    SoundFile *sf = getSound(L);
    luaReturnNum(sf ? sf->GetVolume() : 0.0f);
}

luaFn(sound_isPlaying)
{
    SoundFile *sf = getSound(L);
    luaReturnBool(sf ? sf->IsPlaying() : false);
}



luaFn(music_play)
{
    luaReturnBool(engine->sound->PlayMusic(getCStr(L))); // NULL to resume
}

luaFn(music_stop)
{
    engine->sound->StopMusic();
    luaReturnNil();
}

luaFn(music_pause)
{
    engine->sound->PauseMusic();
    luaReturnNil();
}

luaFn(music_fadeOut)
{
    engine->sound->FadeOutMusic(lua_tonumber(L, 1));
    luaReturnNil();
}

luaFn(music_isPlaying)
{
    luaReturnBool(engine->sound->IsPlayingMusic());
}

luaFn(music_volume)
{
    engine->sound->SetMusicVolume(lua_tonumber(L, 1));
    luaReturnNil();
}

luaFn(music_getVolume)
{
    luaReturnNum(engine->sound->GetMusicVolume());
}

luaFn(music_loopPoint)
{
    engine->sound->SetLoopPoint(lua_tonumber(L, 1));
    luaReturnNil();
}

luaFn(camera_position)
{
    doInterpolateVec2(engine->camera->position, L, 1);
    engine->camera->update(0);
    luaReturnNil();
}

luaFn(camera_offset)
{
    doInterpolateVec2(engine->camera->offset, L, 1);
    engine->camera->update(0);
    luaReturnNil();
}

luaFn(camera_getPosition)
{
    const Vector& cpos = engine->camera->position;
    luaReturnVec2(cpos.x, cpos.y);
}

luaFn(camera_getOffset)
{
    const Vector& cpos = engine->camera->offset;
    luaReturnVec2(cpos.x, cpos.y);
}

luaFn(camera_scale)
{
    doInterpolateVec2(engine->camera->scale, L, 1);
    engine->camera->update(0);
    luaReturnNil();
}

luaFn(camera_getScale)
{
    const Vector& cpos = engine->camera->scale;
    luaReturnVec2(cpos.x, cpos.y);
}

luaFn(camera_getPositionFor)
{
    Vector p = engine->GetCameraPositionFor(Vector(lua_tonumber(L, 1), lua_tonumber(L, 2)));
    luaReturnVec2(p.x, p.y);
}


luaFn(stats_getResourceMem)
{
    luaReturnInt(resMgr.GetUsedMem() / 1024); // scale to KB
}

luaFn(stats_getResourceCount)
{
    luaReturnInt(resMgr.GetUsedCount());
}

luaFn(stats_getRenderedObjects)
{
    luaReturnInt(engine->GetRenderer()->getRenderedObjects());
}

luaFn(stats_getFreeVideoMemory)
{
    luaReturnInt(engine->GetRenderer()->getFreeVideoMemoryKB());
}

luaFn(stats_getRenderedVertices)
{
    luaReturnInt(engine->GetRenderer()->getRenderedVerts());
}

luaFn(stats_getFallbackMem)
{
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetActiveBytes() / 1024);
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetActiveAllocations());
    return 2;
}

luaFn(stats_getFallbackMemDelta)
{
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetBytesDelta());
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetAllocationsDelta());
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetBytesFreedDelta());
    lua_pushinteger(L, Arenas::fallback.GetTracker().GetAllocationsFreedDelta());
    Arenas::fallback.GetTracker().ResetDelta(); // TODO: may not always want to do that here
    return 4;
}

luaFn(stats_getChunkMem)
{
    lua_pushinteger(L, Arenas::chunkAlloc.GetTracker().GetActiveBytes() / 1024);
    lua_pushinteger(L, Arenas::chunkAlloc.GetTracker().GetActiveAllocations());
    return 2;
}

luaFn(stats_getVectorMem)
{
    lua_pushinteger(L, Arenas::vectorInterpArena.GetTracker().GetActiveBytes() / 1024);
    lua_pushinteger(L, Arenas::vectorInterpArena.GetTracker().GetActiveAllocations());
    return 2;
}

luaFn(stats_getQuadMem)
{
    lua_pushinteger(L, Arenas::quadArena.GetTracker().GetActiveBytes() / 1024);
    lua_pushinteger(L, Arenas::quadArena.GetTracker().GetActiveAllocations());
    return 2;
}

luaFn(stats_getEntityMem)
{
    lua_pushinteger(L, Arenas::entityArena.GetTracker().GetActiveBytes() / 1024);
    lua_pushinteger(L, Arenas::entityArena.GetTracker().GetActiveAllocations());
    return 2;
}

luaFn(stats_getObsGridMem)
{
    lua_pushinteger(L, engine->obsgrid.GetMemoryUsage() / 1024);
    lua_pushinteger(L, engine->obsgrid.GetBlocksInUse());
    return 2;
}


luaFn(vec_fromDeg)
{
    Vector v = Vector::FromDegAngle2D(lua_tonumber(L, 1));
    luaReturnVec3(v.x, v.y, 0);
}

luaFn(vec_fromRad)
{
    Vector v = Vector::FromRadAngle2D(lua_tonumber(L, 1));
    luaReturnVec3(v.x, v.y, 0);
}

luaFn(vec_dot)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Vector b(lua_tonumber(L, 3), lua_tonumber(L, 4));
    Vector c = a.dot2D(b);
    luaReturnVec2(c.x, c.y);
}

luaFn(vec_len)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    luaReturnNum(a.getLength2D());
}

luaFn(vec_setLen)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    a.setLength2D(lua_tonumber(L, 3));
    luaReturnVec2(a.x, a.y);
}

luaFn(vec_normalize)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    a.normalize2D();
    luaReturnVec2(a.x, a.y);
}

luaFn(vec_isLenIn)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    luaReturnBool(a.isLength2DIn(lua_tonumber(L, 3)));
}

luaFn(vec_angle)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    float r = a.rotation2D();
    if(r < 0)
        r += 360;
    luaReturnNum(r);
}

luaFn(vec_angleRad)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    float r = a.rotationRad2D();
    if(r < 0)
        r += (2 * PI_F);
    luaReturnNum(r);
}

luaFn(vec_angleBetween)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Vector b(lua_tonumber(L, 3), lua_tonumber(L, 4));
    luaReturnNum(a.angle2D(b));
}

luaFn(vec_angleRadBetween)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Vector b(lua_tonumber(L, 3), lua_tonumber(L, 4));
    luaReturnNum(a.angleRad2D(b));
}


luaFn(vec_reflection)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Vector n(lua_tonumber(L, 3), lua_tonumber(L, 4));
    Vector r = a.getReflection2D(n);
    luaReturnVec2(r.x, r.y);
}

luaFn(vec_rotate)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    a.rotate2D(lua_tonumber(L, 3));
    luaReturnVec2(a.x, a.y);
}

luaFn(vec_rotateRad)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    a.rotateRad2D(lua_tonumber(L, 3));
    luaReturnVec2(a.x, a.y);
}

luaFn(vec_setAngle)
{
    Vector v(lua_tonumber(L, 1), lua_tonumber(L, 2));
    float newa = lua_tonumber(L, 3);
    float a = v.rotation2D();
    newa -= a;
    v.rotate2D(newa);
    luaReturnVec2(v.x, v.y);
}

luaFn(vec_setAngleRad)
{
    Vector v(lua_tonumber(L, 1), lua_tonumber(L, 2));
    float newa = lua_tonumber(L, 3);
    float a = v.rotationRad2D();
    newa -= a;
    v.rotateRad2D(newa);
    luaReturnVec2(v.x, v.y);
}

luaFn(vec_perpendicularRight)
{
    luaReturnVec2(-lua_tonumber(L, 2), lua_tonumber(L, 1));
}

luaFn(vec_perpendicularLeft)
{
    luaReturnVec2(lua_tonumber(L, 2), -lua_tonumber(L, 1));
}

luaFn(vec_projectOnto)
{
    Vector a(lua_tonumber(L, 1), lua_tonumber(L, 2));
    Vector b(lua_tonumber(L, 3), lua_tonumber(L, 4));
    Vector p = a.project2DOnto(b);
    luaReturnVec2(p.x, p.y);
}


static const luaL_Reg statslib[] =
{
    { "getResourceMem", stats_getResourceMem },
    { "getResourceCount", stats_getResourceCount },
    { "getRenderedObjects", stats_getRenderedObjects },
    { "getRenderedVertices", stats_getRenderedVertices },
    { "getFreeVideoMemory", stats_getFreeVideoMemory },
    { "getFallbackMem", stats_getFallbackMem },
    { "getFallbackMemDelta", stats_getFallbackMemDelta },
    { "getChunkMem", stats_getChunkMem },
    { "getVectorMem", stats_getVectorMem },
    { "getQuadMem", stats_getQuadMem },
    { "getEntityMem", stats_getEntityMem },
    { "getObsGridMem", stats_getObsGridMem },
    {NULL, NULL}
};

static const luaL_Reg renderobjectlib[] =
{
    RO_SET_STUFF // Create the call table

    { "delete", ro_delete },
    { "getPosition", ro_getPosition },
    { "parallax", ro_parallax },
    { "blend", ro_blend },
    { "addChild", ro_addChild },
    { "getLayer", ro_getLayer },
    { "isVisible", ro_isVisible },
    { "setPauseLevel", ro_setPauseLevel },
    { "getAbsolutePosition", ro_getAbsolutePosition },
    { "updatePhysics", ro_updatePhysics },
    { "addVel", ro_addVel },
    { "vectorTo", ro_vectorTo },
    { "getParent", ro_getParent },

    // TODO: more

    { NULL, NULL }

};

static const luaL_Reg soundlib[] =
{
    { "new", sound_new },
    { "play", sound_play },
    { "fadeOut", sound_fadeOut },
    { "fadeIn", sound_fadeIn },
    { "stop", sound_stop },
    { "volume", sound_volume },
    { "getVolume", sound_getVolume },
    { "isPlaying", sound_isPlaying },
    { NULL, NULL }
};


static const luaL_Reg musiclib[] =
{
    { "play", music_play },
    { "fadeOut", music_fadeOut },
    { "stop", music_stop },
    { "volume", music_volume },
    { "getVolume", music_getVolume },
    { "isPlaying", music_isPlaying },
    { NULL, NULL }
};

static const luaL_Reg quadlib[] =
{
    { "new", quad_new },
    { "texture", quad_texture },
    { "getWH", quad_getWH }, // TODO: move these to RO
    { "setWH", quad_setWH },
    { "getTexture", quad_getTexture },
    // TODO: more

    {NULL, NULL}
};

static const luaL_Reg entitylib[] =
{
    { "new", entity_new },
    { "setAABBCollider", entity_setAABBCollider },
    { "setCircleCollider", entity_setCircleCollider },
    { "setLineCollider", entity_setLineCollider },
    { "collideWith", entity_collideWith },
    { "collideGrid", entity_collideGrid },
    // TODO: more

    {NULL, NULL}
};

static const luaL_Reg cameralib[] =
{
    { "position", camera_position },
    { "offset", camera_offset },
    { "getPosition", camera_getPosition },
    { "getOffset", camera_getOffset },
    { "scale", camera_scale },
    { "getScale", camera_getScale },
    { "getPositionFor", camera_getPositionFor },

    // TODO: more

    {NULL, NULL}
};

static const luaL_Reg vectorlib[] =
{
    { "fromDeg", vec_fromDeg },
    { "fromRad", vec_fromRad },
    { "dot", vec_dot },
    { "len", vec_len },
    { "setLen", vec_setLen },
    { "normalize", vec_normalize },
    { "isLenIn", vec_isLenIn },
    { "angle", vec_angle },
    { "angleRad", vec_angleRad },
    { "angleBetween", vec_angle },
    { "angleRadBetween", vec_angleRad },
    { "reflection", vec_reflection },
    { "rotate", vec_rotate },
    { "rotateRad", vec_rotateRad },
    { "setAngle", vec_setAngle },
    { "setAngleRad", vec_setAngleRad },
    { "perpendicularLeft", vec_perpendicularLeft },
    { "perpendicularRight", vec_perpendicularRight },
    { "projectOnto", vec_projectOnto },

    {NULL, NULL}
};

int luaopen_renderobject(lua_State *L)
{
    luaL_newlib(L, renderobjectlib);
    return 1;
}

int luaopen_sound(lua_State *L)
{
    luaL_newlib(L, soundlib);
    return 1;
}

int luaopen_music(lua_State *L)
{
    luaL_newlib(L, musiclib);
    return 1;
}

int luaopen_quad(lua_State *L)
{
    luaL_newlib(L, quadlib);
    return 1;
}

int luaopen_entity(lua_State *L)
{
    luaL_newlib(L, entitylib);
    return 1;
}

int luaopen_camera(lua_State *L)
{
    luaL_newlib(L, cameralib);
    return 1;
}

int luaopen_stats(lua_State *L)
{
    luaL_newlib(L, statslib);
    return 1;
}

int luaopen_vector(lua_State *L)
{
    luaL_newlib(L, vectorlib);
    return 1;
}

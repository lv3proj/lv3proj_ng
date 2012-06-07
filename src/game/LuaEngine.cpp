#include "LuaEngine.h"
#include "LuaDefines.h"
#include "LuaInterface.h"
#include "Engine.h"
#include "Quad.h"
#include "MemResource.h"
#include "ResourceMgr.h"
#include "ObjectMgr.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"
#include "Camera.h"
#include "Renderer.h"

struct LuaConstants
{
    const char *name;
    lua_Number val;
};


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
    luaReturnVec2(engine->mouse.x, engine->mouse.y);
}

luaFunc(getMouseWorldPos)
{
    // FIXME: Does it really have to be THAT complicated...?
    Vector campos = engine->camera->position;
    Vector camscale = engine->camera->scale;
    const Vector& rscale = engine->GetRenderer()->getGlobalResolutionScale();
    Vector worldzoom = Vector(1 / (camscale.x * rscale.x), 1 / (camscale.y * rscale.y));
    Vector t = campos + engine->mouse * worldzoom;
    luaReturnVec2(t.x - engine->virtualOffX, t.y - engine->virtualOffY);
}

#include "GL/gl.h"

// HACK: this is just for testing!
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

    glBindTexture(GL_TEXTURE_2D, 0);
    glLineWidth(lw);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);
        glColor4f(r, g, b, a);
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
    glEnd();

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
    luaRegister(getMouseWorldPos),

    { NULL, NULL }
};

static LuaConstants s_consttab[] =
{

    { NULL, 0 }
};


void lua_register_enginefuncs(lua_State *L)
{
    for(unsigned int i = 0; s_functab[i].name; ++i)
    {
        lua_register(L, s_functab[i].name, s_functab[i].func);
    }

    for(unsigned int i = 0; s_consttab[i].name; ++i)
    {
        lua_pushnumber(L, s_consttab[i].val);
        lua_setglobal(L, s_consttab[i].name);
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

static RenderLayer *getLayerByID(lua_State *L, int idx = 1)
{
    if(lua_isnumber(L, idx))
        return engine->layers->GetLayer(lua_tointeger(L, idx));
    else if(lua_isstring(L, idx))
        return engine->layers->GetLayer(getCStr(L, idx));
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

// ScriptObjectUserStruct must be on top of stack; leaves it there.
static void registerScriptObjectStruct(lua_State *L)
{
    // -- Register object in global object table --
    // now [su]
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_touserdata(L, -1);
    lua_getglobal(L, "_OBJECTREGISTRY");
    // now [su][t]
    lua_pushlightuserdata(L, su->obj);
    // now [su][t][q]
    lua_pushvalue(L, -3);
    // now [su][t][q][su]
    lua_settable(L, -3); // t[su->obj] = su
    // now [su][t]
    lua_pop(L, 1);
    // now [su]

    // -- Assign metatable -- (Will be populated in the scripts)
    lua_newtable(L);
    // now [su][t]
    lua_setmetatable(L, -2);
    // now [su]
}

static int registerObject(lua_State *L, ScriptObject *obj)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct*)lua_newuserdata(L, sizeof(ScriptObjectUserStruct));
    obj->scriptBindings = su;
    su->obj = obj;
    su->type = OT_QUAD;
    registerScriptObjectStruct(L);
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

// ----------- Evil define hackery -----------------
#define MAKE_RO_VEC_MTH(name, v, c) \
luaFn(ro_##name) \
{ \
    RenderObject *ro = getRO(L); \
    if(ro) \
        doInterpolateVec##c(ro->v, L, 2); \
    luaReturnSelf(); \
} \

#define RO_SET_STUFF \
MAKE_RO_VEC_MTH(position, position, 2) \
MAKE_RO_VEC_MTH(offset, offset, 2) \
MAKE_RO_VEC_MTH(scale, scale, 2) \
MAKE_RO_VEC_MTH(color, color, 3) \
MAKE_RO_VEC_MTH(alpha, alpha, 1) \
MAKE_RO_VEC_MTH(alpha2, alpha2, 1) \
MAKE_RO_VEC_MTH(rotate, rotation, 1) \
MAKE_RO_VEC_MTH(rotate2, rotation2, 1) \
MAKE_RO_VEC_MTH(velocity, velocity, 2) \
MAKE_RO_VEC_MTH(gravity, gravity, 2)

RO_SET_STUFF // Create the functions

luaFn(ro_getPosition)
{
    RenderObject *ro = getRO(L);
    if(ro)
        luaReturnVec2(ro->position.x, ro->position.y);
    
    luaReturnVec2(0, 0);
}

#undef MAKE_RO_VEC_MTH

#define MAKE_RO_VEC_MTH(name, v, c){ #name, ro_##name },

//-------------------------------------------


luaFn(quad_new)
{
    RenderLayer *lr = getLayerByID(L, 2);
    if(!lr)
    {
        // TODO: nicer output, add stack
        logerror("ERROR: quad:new() invalid target layer");
        luaReturnNil();
    }

    Quad *q = new Quad(getCStr(L, 1), lua_tointeger(L, 3), lua_tointeger(L, 4));
    lr->Add(q);
    return registerObject(L, q);
}

static const luaL_Reg renderobjectlib[] =
{
    RO_SET_STUFF // Create the call table

    { "delete", ro_delete },
    { "getPosition", ro_getPosition },

    // TODO: more

    { NULL, NULL }

};



static const luaL_Reg quadlib[] =
{
    {"new", quad_new},
    // TODO: more

    {NULL, NULL}
};


int luaopen_renderobject(lua_State *L)
{
    luaL_newlib(L, renderobjectlib);
    return 1;
}

int luaopen_quad(lua_State *L)
{
    luaL_newlib(L, quadlib);
    return 1;
}

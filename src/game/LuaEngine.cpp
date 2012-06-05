#include "LuaEngine.h"
#include "LuaDefines.h"
#include "LuaInterface.h"
#include "Engine.h"
#include "Quad.h"
#include "MemResource.h"
#include "ResourceMgr.h"
#include "ObjectMgr.h"

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

static LuaFunctions s_functab[] =
{
    { "dofile", l_dofile_wrap },
    { "loadfile", l_loadfile_wrap },
    luaRegister(wait),

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
#define MAKE_RO_VEC_MTH(name, v) \
luaFunc(ro_##name) \
{ \
    RenderObject *ro = getRO(L); \
    if(ro) \
        doInterpolateVec2(ro->v, L, 2); \
    luaReturnSelf(); \
}

#define RO_SET_STUFF \
MAKE_RO_VEC_MTH(position, position) \
MAKE_RO_VEC_MTH(offset, offset) \
MAKE_RO_VEC_MTH(scale, scale) \
MAKE_RO_VEC_MTH(color, color) \
MAKE_RO_VEC_MTH(alpha, alpha) \
MAKE_RO_VEC_MTH(alpha2, alpha2) \
MAKE_RO_VEC_MTH(rotate, rotation) \
MAKE_RO_VEC_MTH(rotate2, rotation2) \
MAKE_RO_VEC_MTH(velocity, velocity) \
MAKE_RO_VEC_MTH(gravity, gravity)

RO_SET_STUFF // Create the functions

#undef MAKE_RO_VEC_MTH

#define MAKE_RO_VEC_MTH(name, v){ #name, l_ro_##name },

//-------------------------------------------


luaFn(quad_new)
{
    return registerObject(L, new Quad(getCStr(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3)));
}

static const luaL_Reg renderobjectlib[] =
{
    { "delete", ro_delete },
    RO_SET_STUFF // Create the call table
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

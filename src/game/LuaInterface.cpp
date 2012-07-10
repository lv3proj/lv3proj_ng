#include "LuaInterface.h"
#include "LuaDefines.h"
#include "MemResource.h"
#include "ResourceMgr.h"
#include <assert.h>
#include <sstream>

#include "LuaEngine.h"
#include "LuaConstants.h"


LuaInterface::LuaInterface()
 : _lua(NULL)
{
}

LuaInterface::~LuaInterface()
{
    Shutdown();
}

void LuaInterface::Shutdown()
{
    logdebug("LuaInterface::Shutdown()");
    lua_close(_lua);
    _lua = NULL;
}

void LuaInterface::GC()
{
    unsigned int before = MemUsed();
    lua_gc(_lua, LUA_GCCOLLECT, 0);
    unsigned int after = MemUsed();
    logdebug("GC: before: %u KB, after: %u KB", before, after);
}

unsigned int LuaInterface::MemUsed()
{
    return lua_gc(_lua, LUA_GCCOUNT, 0);
}

static const luaL_Reg customLibs[] = {
    {"ro", luaopen_renderobject},
    {"quad", luaopen_quad},
    {"entity", luaopen_entity},
    {"sound", luaopen_sound},
    {"music", luaopen_music},
    {"camera", luaopen_camera},
    {"stats", luaopen_stats},
    {NULL, NULL}
};

bool LuaInterface::Init()
{
    if(!_lua)
    {
        _lua = luaL_newstate();

        // Lua internal functions
        luaL_openlibs(_lua);

        // Own functions.
        lua_register_enginefuncs(_lua);

        /* call open functions from 'customLibs' and set results to global table */
        for (const luaL_Reg *lib = customLibs; lib->func; lib++) {
            luaL_requiref(_lua, lib->name, lib->func, 1);
            lua_pop(_lua, 1);  /* remove lib */
        }

        lua_register_constants(_lua);
    }

    // Execute scripts/init.lua
    lua_getglobal(_lua, "dofile");
    lua_pushstring(_lua, "init.lua");
    if(lua_pcall(_lua, 1, 0, 0) != LUA_OK)
    {
        logerror("Could not run init script 'scripts/init.lua'");
        logerror("-> %s", getCStr(_lua, -1));
        lua_pop(_lua, 1);
        return false;
    }

    return true;
}

void LuaInterface::UnregisterObject(ScriptObject *obj)
{
    ScriptObjectUserStruct *su = (ScriptObjectUserStruct *)obj->scriptBindings;
    if(!su)
        return;

    assert(su->obj == obj);
    assert(obj->scriptBindings == su);
    obj->scriptBindings = NULL;
    su->obj = NULL;
    su->type = OT_NONE;

    deleteUserdata(_lua, obj);
}

void registerUserdata(lua_State *L, void *ptr)
{
    // now [obj]
    lua_pushlightuserdata(L, ptr);
    // now [obj][ptr]
    lua_pushvalue(L, -2);
    // now [obj][ptr][obj]
    lua_settable(L, LUA_REGISTRYINDEX); // REG[ptr] = obj
    // now [obj]
}

void deleteUserdata(lua_State *L, void *ptr)
{
    lua_pushlightuserdata(L, ptr);
    // now [key]
    lua_pushnil(L);
    // now [key][nil]
    lua_settable(L, LUA_REGISTRYINDEX);
}

void lookupUserdata(lua_State *L, void *ptr)
{
    lua_pushlightuserdata(L, ptr);
    // now [obj]
    lua_gettable(L, LUA_REGISTRYINDEX);
    // now [su]
}

static std::string luaFormatStackInfo(lua_State *L, int level = 1)
{
    lua_Debug ar;
    std::ostringstream os;
    if (lua_getstack(L, level, &ar) && lua_getinfo(L, "Sln", &ar))
    {
        os << ar.short_src << ":" << ar.currentline
            << " ([" << ar.what << "] "  << ar.namewhat << " " << (ar.name ? ar.name : "(?)") << ")";
    }
    else
    {
        os << "???:0";
    }

    return os.str();
}

static void printCallstack(lua_State *L)
{
    lua_Debug dummy;
    for (int level = 0; lua_getstack(L, level, &dummy); ++level)
        logerror("%s", luaFormatStackInfo(L, level).c_str());
}

void LuaInterface::lookupFunc(const char *f)
{
    lua_getglobal(_lua, f);
}

bool LuaInterface::doCall(int nparams, int nrets /* = 0 */)
{
    if(lua_pcall(_lua, nparams, nrets, 0) != LUA_OK)
    {
        logerror("Lua: %s", getCStr(_lua, -1));
        printCallstack(_lua);
        lua_pop(_lua, 1);
        return false;
    }
    return true;
}

bool LuaInterface::call(const char *func)
{
    lookupFunc(func);
    return doCall(0);
}

bool LuaInterface::call(const char *func, float f)
{
    lookupFunc(func);
    lua_pushnumber(_lua, f);
    return doCall(1);
}

bool LuaInterface::call(const char *func, int a, int b)
{
    lookupFunc(func);
    lua_pushinteger(_lua, a);
    lua_pushinteger(_lua, b);
    return doCall(2);
}

void LuaInterface::lookupMethod(ScriptObject *obj, const char *func)
{
    lookupUserdata(_lua, obj);
    // now [su]
    lua_pushstring(_lua, func);
    // now [su]["func"]
    lua_gettable(_lua, -2); // invokes metamethod
    // now [su][func]
    lua_insert(_lua, -2);
    // now [func][su]
}

bool LuaInterface::callMethod(ScriptObject *obj, const char *func)
{
    lookupMethod(obj, func);
    return doCall(1);
}

bool LuaInterface::callMethod(ScriptObject *obj, const char *func, float f)
{
    lookupMethod(obj, func);
    lua_pushnumber(_lua, f);
    return doCall(2);
}

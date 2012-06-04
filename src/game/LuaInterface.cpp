#include "LuaInterface.h"
#include "LuaDefines.h"
#include "MemResource.h"
#include "ResourceMgr.h"

#include "LuaEngine.h"


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
    lua_close(_lua);
    _lua = NULL;
}

void LuaInterface::GC()
{
    lua_gc(_lua, LUA_GCCOLLECT, 0);
}

unsigned int LuaInterface::MemUsed()
{
    return lua_gc(_lua, LUA_GCCOUNT, 0);
}

bool LuaInterface::Init()
{
    if(!_lua)
    {
        _lua = luaL_newstate();

        // Lua internal functions
        luaopen_base(_lua);
        luaopen_table(_lua);
        luaopen_string(_lua);
        luaopen_math(_lua);
        luaopen_debug(_lua);
        luaopen_coroutine(_lua);
        luaopen_bit32(_lua);
        luaopen_io(_lua);

        // Own functions.
        lua_register_engine(this);
    }

    // Execute scripts/init.lua
    lua_getglobal(_lua, "dofile");
    lua_pushstring(_lua, "init.lua");
    if(lua_pcall(_lua, 1, 0, 0) != LUA_OK)
    {
        logerror("Could not find initialization script 'scripts/init.lua'");
        return false;
    }

    return true;
}
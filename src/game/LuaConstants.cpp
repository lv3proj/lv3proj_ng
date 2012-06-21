#include "LuaConstants.h"
#include "ScriptedEngine.h"
#include "RenderObject.h"

struct LuaConstants
{
    const char *name;
    lua_Number val;
};

LuaConstants constants[] =
{
    luaConstant(BLEND_NONE),
    luaConstant(BLEND_DEFAULT),
    luaConstant(BLEND_ADD),
    luaConstant(BLEND_SUB),
    luaConstant(BLEND_MULT),

    {NULL, 0}
};

int lua_register_constants(lua_State *L)
{
    for (unsigned int i = 0; constants[i].name; i++)
    {
        lua_pushnumber(L, constants[i].val);
        lua_setglobal(L, constants[i].name);
    }
    return 0;
}

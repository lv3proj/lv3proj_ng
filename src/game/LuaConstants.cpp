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
    // blend modes
    luaConstant(BLEND_NONE),
    luaConstant(BLEND_DEFAULT),
    luaConstant(BLEND_ADD),
    luaConstant(BLEND_SUB),
    luaConstant(BLEND_MULT),

    luaConstant(SDL_JOYAXISMOTION),
    luaConstant(SDL_JOYBALLMOTION),
    luaConstant(SDL_JOYHATMOTION),
    luaConstant(SDL_JOYBUTTONDOWN),
    luaConstant(SDL_JOYBUTTONUP),

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

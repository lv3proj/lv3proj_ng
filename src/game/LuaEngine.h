#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

#include "ScriptObject.h"

struct lua_State;

void lua_register_enginefuncs(lua_State *);
int luaopen_renderobject(lua_State *);
int luaopen_quad(lua_State *);
int luaopen_entity(lua_State *);
int luaopen_sound(lua_State *);
int luaopen_music(lua_State *);
int luaopen_camera(lua_State *);
int luaopen_stats(lua_State *);

struct ScriptObjectUserStruct
{
    ScriptObject *obj;
    ScriptObjectType type;
};


#endif


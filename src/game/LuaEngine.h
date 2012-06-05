#ifndef LUA_ENGINE_BINDINGS_H
#define LUA_ENGINE_BINDINGS_H

#include "ScriptObject.h"

struct lua_State;

void lua_register_enginefuncs(lua_State *);
int luaopen_renderobject(lua_State *);
int luaopen_quad(lua_State *);


struct ScriptObjectUserStruct
{
    ScriptObject *obj;
    ScriptObjectType type;
};


#endif


#ifndef LUA_MY_DEFINES_H
#define LUA_MY_DEFINES_H

#include <lua/lua.hpp>
#include <string>


#define luaFunc(func)		static int l_##func(lua_State *L)
#define luaRegister(func)	{#func, l_##func}

#define luaReturnNil() { return 0; }
#define luaReturnNum(x) { lua_pushnumber(L, x); return 1; }

inline const char *getCStrSafe(lua_State *L, int idx = 1)
{
    const char *s = NULL;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s ? s : "";
}

inline const char *getCStr(lua_State *L, int idx = 1)
{
    const char *s = NULL;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s;
}

inline std::string getStr(lua_State *L, int idx = 1)
{
    std::string s;
    if(lua_isstring(L, idx))
        s = lua_tostring(L, idx);
    return s;
}





#endif

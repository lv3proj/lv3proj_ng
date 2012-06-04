#include "LuaDefines.h"
#include "LuaInterface.h"
#include "Engine.h"
#include "Quad.h"
#include "MemResource.h"
#include "ResourceMgr.h"

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

static LuaFunctions s_functab[] =
{
    { "dofile", l_dofile_wrap },
    { "loadfile", l_loadfile_wrap },

    { NULL, NULL }
};

static LuaConstants s_consttab[] =
{

    { NULL, 0 }
};


void lua_register_engine(LuaInterface *S)
{
    lua_State *L = S->getState();

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

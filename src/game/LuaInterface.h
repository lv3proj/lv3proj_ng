#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

#include "common.h"

// TODO: derive from ScriptInterface base class,
// that provides virtual interfaces?

struct lua_State;

class LuaInterface
{
public:

    LuaInterface();
    ~LuaInterface();
    
    bool Init();
    void Shutdown();
    void GC();
    unsigned int MemUsed();

    void AddClass(const char *name);
    void AddClassMethod(const char *name);
    void AddClassMember(const char *name);

    inline lua_State *getState() const { return _lua; }



protected:

    lua_State *_lua;
};


#endif


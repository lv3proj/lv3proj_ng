#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

#include "common.h"

// TODO: derive from ScriptInterface base class,
// that provides virtual interfaces?

struct lua_State;
class ScriptObject;

class LuaInterface
{
public:

    LuaInterface();
    ~LuaInterface();
    
    bool Init();
    void Shutdown();
    void GC();
    unsigned int MemUsed();
    void UnregisterObject(ScriptObject *obj);

    inline lua_State *getState() const { return _lua; }

    bool call(const char *f);
    bool call(const char *f, float);



protected:

    void lookupFunc(const char *f);
    bool doCall(int nparams, int nrets = 0);

    lua_State *_lua;
};


#endif


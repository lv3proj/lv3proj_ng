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
    bool call(const char *f, int, int);

    bool callMethod(ScriptObject *, const char *f);
    bool callMethod(ScriptObject *, const char *f, float);


protected:

    void lookupFunc(const char *f);
    void lookupMethod(ScriptObject *obj, const char *f);
    bool doCall(int nparams, int nrets = 0);

    lua_State *_lua;
};

// registers the object on top of the stack having index 'ptr'
// leaves object on the stack.
void registerUserdata(lua_State *L, void *ptr);

// looks up userdata associated with 'ptr'
void lookupUserdata(lua_State *L, void *ptr);

// unreferences userdata associated with 'ptr'
void deleteUserdata(lua_State *L, void *ptr);


#endif


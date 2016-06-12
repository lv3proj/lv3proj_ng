#ifndef LUA_INTERFACE_H
#define LUA_INTERFACE_H

#include "common.h"
#include <map>
#include "MemoryAllocatorSmallBlock.h"

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
    unsigned int MemUsedKB();
    //void UnregisterObject(ScriptObject *obj);

    inline lua_State *getState() const { return _lua; }

    bool call(const char *f);
    bool call(const char *f, float);
    bool call(const char *f, int, int);
    bool call(const char *f, int, int, int, int);

    //bool callMethod(ScriptObject *, const char *f);
    //bool callMethod(ScriptObject *, const char *f, float);

    // for debugging -- remove later
    //typedef std::map<unsigned int, unsigned int> LuaAllocStats;

    static void *the_alloc(void *ud, void *ptr, size_t osize, size_t nsize);

protected:

    void lookupFunc(const char *f);
    //void lookupMethod(ScriptObject *obj, const char *f);
    bool doCall(int nparams, int nrets = 0);

    lua_State *_lua;
    SmallBlockAllocator _sballoc;
    //LuaAllocStats _stats;
};

/*
// registers the object on top of the stack having index 'ptr'
// leaves object on the stack.
void registerUserdata(lua_State *L, void *ptr);

// looks up userdata associated with 'ptr'
void lookupUserdata(lua_State *L, void *ptr);

// unreferences userdata associated with 'ptr'
void deleteUserdata(lua_State *L, void *ptr);
*/

#endif


#ifndef SCRIPTEDENTITY_H
#define SCRIPTEDENTITY_H

#include "Entity.h"

class LuaInterface;

class ScriptedEntity : public Entity
{
public:
    static ScriptedEntity *create(LuaInterface *script);
    virtual void destroy();

    // DO NOT USE - only used by memory system
    virtual ~ScriptedEntity();

    virtual void update(float dt);
    virtual void updateFixed(float dt);
    virtual void onEndOfLife();



protected:
    inline ScriptedEntity(LuaInterface *script)
        : _script(script)
    {
    }



    LuaInterface *_script;
};

#endif

#ifndef SCRIPTEDENTITY_H
#define SCRIPTEDENTITY_H

#include "Entity.h"

class LuaInterface;

class ScriptedEntity : public Entity
{
public:
    ScriptedEntity(LuaInterface *script);
    virtual ~ScriptedEntity();

    virtual void update(float dt);

protected:

    LuaInterface *_script;
};

#endif

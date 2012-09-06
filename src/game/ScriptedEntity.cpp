#include "ScriptedEntity.h"
#include "LuaInterface.h"


ScriptedEntity::ScriptedEntity(LuaInterface *sc)
 : _script(sc)
{
}

ScriptedEntity::~ScriptedEntity()
{
}

void ScriptedEntity::update(float dt)
{
    _script->callMethod(this, "update", dt);

    Entity::update(dt);
}

void ScriptedEntity::updateFixed(float dt)
{
    _script->callMethod(this, "updateFixed", dt);

    Entity::updateFixed(dt);
}

void ScriptedEntity::onEndOfLife()
{
    _script->callMethod(this, "onEndOfLife");
    Entity::onEndOfLife();
}


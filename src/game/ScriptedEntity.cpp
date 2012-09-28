#include "ScriptedEntity.h"
#include "LuaInterface.h"
#include "Arenas.h"

namespace Arenas {
    Arenas::EntityMem entityArena(Arenas::chunkAlloc, 1000, sizeof(ScriptedEntity) );
};

ScriptedEntity *ScriptedEntity::create(LuaInterface *script)
{
    return XNEW(ScriptedEntity, Arenas::entityArena)(script);
}

void ScriptedEntity::destroy()
{
    XDELETE_NN(this, Arenas::entityArena);
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


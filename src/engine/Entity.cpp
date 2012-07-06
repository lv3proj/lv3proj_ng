#include "Entity.h"
#include "Engine.h"


Entity::Entity()
 : _pauseLevel(1)
{
}

Entity::~Entity()
{
}

bool Entity::isPaused() const
{
    return engine->IsPause(_pauseLevel);
}


#include "ScriptObject.h"
#include "Engine.h"

ScriptObject::ScriptObject()
: scriptBindings(NULL)
, _dead(false)
, _managed(false)
 , _pauseLevel(0)
{
}

ScriptObject::~ScriptObject()
{
}

bool ScriptObject::isPaused() const
{
    return engine->IsPause(_pauseLevel);
}


LifeObject::LifeObject()
: _life(1)
 , _decay(0)

{
    _managed = true;
}

LifeObject::~LifeObject()
{
}

void LifeObject::update(float dt)
{
    if(!isDead())
    {
        ScriptObject::update(dt);
        _life -= (_decay * dt);
        if(_life <= 0)
            onEndOfLife();
    }
}

void LifeObject::onEndOfLife()
{
    if(!_dead)
    {
        _dead = true;
        engine->UnregisterObject(this);
    }
}

void LifeObject::kill(float decay)
{
    if(decay <= 0)
        _life = -1;
    else
        _decay = decay;
}

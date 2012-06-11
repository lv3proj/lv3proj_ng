#include "ScriptObject.h"
#include "Engine.h"

ScriptObject::ScriptObject()
: scriptBindings(NULL)
, _dead(false)
, _managed(false)
{
}

ScriptObject::~ScriptObject()
{
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
    _dead = true;
    engine->UnregisterObject(this);
}

void LifeObject::kill(float decay)
{
    if(decay <= 0)
        _life = -1;
    else
        _decay = decay;
}


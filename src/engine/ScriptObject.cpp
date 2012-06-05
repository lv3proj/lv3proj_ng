#include "ScriptObject.h"
#include "Engine.h"

ScriptObject::ScriptObject()
 : scriptBindings(NULL)
 , _dead(false)
 , _life(1)
 , _decay(0)
{
}

ScriptObject::~ScriptObject()
{
}

void ScriptObject::update(float dt)
{
    if(!_dead)
    {
        _life -= (_decay * dt);
        if(_life <= 0)
            onEndOfLife();
    }
}

void ScriptObject::onEndOfLife()
{
    _dead = true;
    engine->UnregisterObject(this);
}

void ScriptObject::kill(float decay)
{
    if(decay <= 0)
        _life = -1;
    else
        _decay = decay;
}


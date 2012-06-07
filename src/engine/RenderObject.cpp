#include "RenderObject.h"
#include "Engine.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"


RenderObject::RenderObject()
{
    _layer = LR_INVALID;
    _blend = BLEND_DEFAULT;
    scale = Vector(1, 1, 1);
    color = Vector(1, 1, 1);
    alpha = 1;
    alpha2 = 1;
}

RenderObject::~RenderObject()
{
}

void RenderObject::update(float dt)
{
    ScriptObject::update(dt);

    position.update(dt);
    offset.update(dt);
    scale.update(dt);
    color.update(dt);
    alpha.update(dt);
    alpha2.update(dt);
    rotation.update(dt);
    rotation2.update(dt);
    velocity.update(dt);
    gravity.update(dt);

}

void RenderObject::onEndOfLife()
{
    ScriptObject::onEndOfLife();

    engine->layers->GetLayer(getLayer())->Remove(this);
}

void RenderObject::toLayer(unsigned int target)
{
    if(_layer == target)
        return;
    engine->layers->GetLayer(getLayer())->Remove(this);
    _layer = LR_INVALID; // to avoid warnings
    engine->layers->GetLayer(target)->Add(this);
    _layer = target;
}

void RenderObject::moveToBack()
{
    engine->layers->GetLayer(getLayer())->MoveToBack(this);
}

void RenderObject::moveToFront()
{
    engine->layers->GetLayer(getLayer())->MoveToFront(this);
}


#include "RenderObject.h"
#include "Engine.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Camera.h"

float RenderObject::s_cullX1 = 0;
float RenderObject::s_cullX2 = 0;
float RenderObject::s_cullY1 = 0;
float RenderObject::s_cullY2 = 0;


RenderObject::RenderObject()
    : width(0)
    , height(0)
    , halfWidth(0)
    , halfHeight(0)
    , _layer(LR_INVALID)
    , _blend(BLEND_DEFAULT)
    , scale(1, 1, 1)
    , color(1, 1, 1)
    , color2(1, 1, 1)
    , alpha(1)
    , alpha2(1)
    , parallax(1, 1, 1)
    , _parent(NULL)
    , _layerPtr(NULL)
    , _noCull(false)
{
}

RenderObject::~RenderObject()
{
}

void RenderObject::update(float dt)
{
    LifeObject::update(dt);

    position.update(dt);
    offset.update(dt);
    scale.update(dt);
    color.update(dt);
    color2.update(dt);
    alpha.update(dt);
    alpha2.update(dt);
    rotation.update(dt);
    rotation2.update(dt);
    velocity.update(dt);
    gravity.update(dt);
}

void RenderObject::onEndOfLife()
{
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        RenderObject *child = *it;
        child->_parent = NULL;
        child->onEndOfLife();
    }
    _children.clear();

    if(_parent)
    {
        _parent->removeChild(this);
    }


    LifeObject::onEndOfLife();

    engine->layers->GetLayer(getLayer())->Remove(this);
}

void RenderObject::toLayer(unsigned int target)
{
    if(_layer == target)
        return;
    engine->layers->GetLayer(getLayer())->Remove(this);
    _layer = LR_INVALID; // to avoid warnings
    _layerPtr = engine->layers->GetLayer(target);
    _layerPtr->Add(this);
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

void RenderObject::addChild(RenderObject *child)
{
    if(child == this || child == _parent)
    {
        logerror("addChild gone wrong!");
        return;
    }

    if(child->_parent)
    {
        child->_parent->removeChild(child);
    }

    child->_parent = this;
    _children.insert(child);
}

void RenderObject::removeChild(RenderObject *child)
{
    if(child->_parent == this)
    {
        _children.erase(child);
        child->_parent = NULL;
    }
}

bool RenderObject::isVisible() const
{
    if(!alpha.x)
        return false;

    if(_parent && !_parent->isVisible())
        return false;

    return true;
}

Vector RenderObject::getAbsolutePosition() const
{
    Vector p = position;
    for(RenderObject *pa = _parent; pa; pa = pa->_parent)
        p += pa->position;
    return p;
}

bool RenderObject::isOnScreen() const
{
    if(_noCull)
        return true;

    const Vector p = getParallaxRenderPosition(engine->camera->screenCenter);
    const float ws = halfWidth * scale.x;
    const float hs = halfHeight * scale.y;
    return p.x >= s_cullX1 - ws
        && p.x <= s_cullX2 + ws
        && p.y >= s_cullY1 - hs
        && p.y <= s_cullY2 + hs;
}


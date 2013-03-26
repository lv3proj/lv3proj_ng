#include "RenderObject.h"
#include "Engine.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "Camera.h"
#include <algorithm>

float RenderObject::s_cullX1 = 0;
float RenderObject::s_cullX2 = 0;
float RenderObject::s_cullY1 = 0;
float RenderObject::s_cullY2 = 0;


RenderObject::RenderObject()
    : width(0)
    , height(0)
    , halfWidth(0)
    , halfHeight(0)
    , _blend(BLEND_DEFAULT)
    , scale(1, 1, 1)
    , scale2(1, 1, 1)
    , color(1, 1, 1)
    , color2(1, 1, 1)
    , alpha(1)
    , alpha2(1)
    , parallax(1, 1, 1)
    , _parent(NULL)
    , _layerPtr(NULL)
    , _noCull(false)
    , _layerBlock(NULL)
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
    scale2.update(dt);
    color.update(dt);
    color2.update(dt);
    alpha.update(dt);
    alpha2.update(dt);
    rotation.update(dt);
    rotation2.update(dt);
    velocity.update(dt);
    gravity.update(dt);
    friction.update(dt);

    onUpdate(dt);

    for(Children::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        RenderObject *ro = *it;
        if(ro->_objIndex == -1 && !(ro->isDead() || ro->isPaused()))
            ro->update(dt);
    }
}

void RenderObject::updateFixed(float dt)
{
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        RenderObject *ro = *it;
        if(ro->_objIndex == -1 && !(ro->isDead() || ro->isPaused()))
            ro->updateFixed(dt);
    }
}

void RenderObject::onUpdate(float dt)
{
    updatePhysics(dt);
}

void RenderObject::updatePhysics(float dt)
{
    position += velocity * dt;
    velocity += gravity * dt;

    if(!velocity.isZero())
    {
        float t = velocity.x;
        if(t < 0)
        {
            t += friction.x * dt;
            if(t < 0)
                velocity.x = t;
            else
                velocity.x = 0;
        }
        else
        {
            t -= friction.x * dt;
            if(t > 0)
                velocity.x = t;
            else
                velocity.x = 0;
        }

        t = velocity.y;
        if(t < 0)
        {
            t += friction.y * dt;
            if(t < 0)
                velocity.y = t;
            else
                velocity.y = 0;
        }
        else
        {
            t -= friction.y * dt;
            if(t > 0)
                velocity.y = t;
            else
                velocity.y = 0;
        }
    }
}


void RenderObject::onEndOfLife()
{
    //if(_dead)
    //	return;

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

    if(_layerPtr)
        _layerPtr->Remove(this);
    //else
    //    logdev("WARNING: Object %p not on any layer", this);

    // If not registered in objmgr, delete right now
    //if(_objIndex == -1)
    //	this->destroy();
}

void RenderObject::toLayer(unsigned int target)
{
    if(_layerPtr)
    {
        if(_layerPtr->GetID() == target)
            return;
        _layerPtr->Remove(this);
    }
    engine->layers->GetLayer(target)->Add(this);
}

void RenderObject::moveToBack()
{
    _layerPtr->MoveToBack(this);
}

void RenderObject::moveToFront()
{
    _layerPtr->MoveToFront(this);
}

void RenderObject::addChild(RenderObject *child)
{
    if(child->_parent == this)
        return;

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
    _children.push_back(child);
}

void RenderObject::removeChild(RenderObject *child)
{
    if(child->_parent == this)
    {
        Children::iterator it = std::find(_children.begin(), _children.end(), child);
        ASSERT(it != _children.end());
        _children.erase(it);
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

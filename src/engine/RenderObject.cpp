#include "RenderObject.h"
#include "Engine.h"
#include "RenderLayer.h"
#include "RenderLayerMgr.h"
#include "collision/AABB.h"
#include "collision/Circle.h"


RenderObject::RenderObject()
{
    _layer = LR_INVALID;
    _blend = BLEND_DEFAULT;
    scale = Vector(1, 1, 1);
    color = Vector(1, 1, 1);
    alpha = 1;
    alpha2 = 1;
    parallax = Vector(1, 1, 1);
    collider = NULL;
    _parent = NULL;
    _layerPtr = NULL;
}

RenderObject::~RenderObject()
{
    delete collider;
}

void RenderObject::update(float dt)
{
    LifeObject::update(dt);

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

    if(collider)
    {
        switch(collider->getShape())
        {
            case COLL_AABB:
                ((AABB*)collider)->setCenter(position + offset);
                break;

            case COLL_CIRCLE:
                ((Circle*)collider)->position = position + offset;
                break;

            case COLL_LINE:
            case COLL_PIXMAP:
            default:
                assert(false); // Not sure what to do here
        }
    }

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
    if(!(alpha.x && alpha2.x))
        return false;

    if(_parent && !_parent->isVisible())
        return false;

    return true;
}


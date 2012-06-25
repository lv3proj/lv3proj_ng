#include "common.h"
#include "RenderLayer.h"
#include "RenderObject.h"
#include "Renderer.h"
#include "Engine.h"

RenderLayer::RenderLayer()
 : _id(-1), noCamera(false)
{
}

RenderLayer::~RenderLayer()
{
    RemoveAll();
}

void RenderLayer::RemoveAll()
{
    tiles.Clear();

    if(_objs.empty())
        return;

    logdebug("WARNING: ~RenderLayer(%u), sending %u objects to layer 0", _id, (unsigned int)_objs.size());
    while(!_objs.empty())
    {
        RenderObject *ro = _objs.front();
        if(ro->getLayer())
            ro->toLayer(0); // this also erases from current layer
        else
        {
            ro->kill(); // FIXME: really?
            Remove(ro);
        }
    }
}

void RenderLayer::Add(RenderObject *ro)
{
    if(ro->getLayer())
        logerror("RenderLayer(%u)::Add: Object %p already has a layer set, may go haywire later!", _id, ro);
    _objs.push_back(ro);
    ro->_layer = _id;
}

void RenderLayer::AddFront(RenderObject *ro)
{
    if(ro->getLayer())
        logerror("RenderLayer(%u)::AddFront: Object %p already has a layer set (%u), may go haywire later!", _id, ro, ro->getLayer());
    _objs.push_front(ro);
    ro->_layer = _id;
}

void RenderLayer::Remove(RenderObject *ro)
{
    if(ro->getLayer() != _id)
        logerror("RenderLayer(%u)::Remove: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayer());
    _objs.remove(ro);
    ro->_layer = LR_INVALID;
}

void RenderLayer::MoveToBack(RenderObject *ro)
{
    if(ro->getLayer() != _id)
        logerror("RenderLayer(%u)::MoveToBack: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayer());
    _objs.remove(ro);
    _objs.push_front(ro); // rendered first -> in back
}

void RenderLayer::MoveToFront(RenderObject *ro)
{
    if(ro->getLayer() != _id)
        logerror("RenderLayer::MoveToFront: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayer());
    _objs.remove(ro);
    _objs.push_back(ro); // rendered last -> in front
}

void RenderLayer::Render()
{
    Renderer *r = engine->GetRenderer();
    
    if(noCamera)
        r->loadIdentity();
    else
        r->setupRenderPositionAndScale();

    tiles.onRender();

    for(std::list<RenderObject*>::const_iterator it = _objs.begin(); it != _objs.end(); ++it)
    {
        RenderObject *ro = *it;
        if(!ro->getParent())
            r->renderObject(*it);
    }
}

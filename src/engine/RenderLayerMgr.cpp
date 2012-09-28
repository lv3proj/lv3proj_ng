#include "RenderLayerMgr.h"
#include "RenderLayer.h"


RenderLayerMgr::RenderLayerMgr()
{
    _layers.resize(32);

    for(size_t i = 0; i < _layers.size(); ++i)
    {
        _layers[i] = new RenderLayer(i);
    }
}

RenderLayerMgr::~RenderLayerMgr()
{
    ClearAll();
    for(size_t i = 1; i < _layers.size(); ++i)
        delete _layers[i];
    delete _layers[0];
}

void RenderLayerMgr::ClearAll()
{
    for(size_t i = 1; i < _layers.size(); ++i)
        _layers[i]->RemoveAll(); // this just drops them, the actual work is done by objmgr.
    _layers[0]->RemoveAll();
}

void RenderLayerMgr::Render()
{
    // Not rendering layer 0 is intentional.
    // It serves as a parking space for unused objects.
    for(size_t i = 1; i < _layers.size(); ++i)
        _layers[i]->Render();
}

RenderLayer *RenderLayerMgr::GetLayer(const char *name)
{
    for(size_t i = 1; i < _layers.size(); ++i)
        if(_layers[i]->name == name)
            return _layers[i];
    return NULL;
}


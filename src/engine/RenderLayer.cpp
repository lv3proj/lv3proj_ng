#include "common.h"
#include "RenderLayer.h"
#include "TileGrid.h"
#include "RenderObject.h"
#include "Renderer.h"
#include "Engine.h"
#include "Arenas.h"


void RenderLayer::Block::insert(RenderObject *ro)
{
    ASSERT(hasSpace());
    ASSERT(ro->_layerBlock == NULL);
    ASSERT(ptrs[wIdx] ==  NULL);
    ro->_layerBlock = this;
    ro->_indexInBlock = wIdx;
    ptrs[wIdx] = ro;
    wIdx += direction;
    ++objCount;
}

void RenderLayer::Block::remove(RenderObject *ro)
{
    const unsigned int idx = ro->_indexInBlock;

    ASSERT(ro->_layerBlock == this);
    ASSERT(ptrs[idx] == ro);

    ptrs[idx] = NULL;
    --objCount;

    // Recycle last insert position
    // (ie. if an object was appended to end, and is now moved away)
    if(idx + direction == wIdx)
        wIdx = idx;

    ro->_layerBlock = NULL;
}


RenderLayer::RenderLayer(unsigned int id)
 : _id(id)
 , parallax(1, 1)
 , _firstBlock(NULL)
 , _lastBlock(NULL)
 , _objectCount(0)
{
    tiles = new TileGrid();
    tiles->_layerPtr = this;
}

RenderLayer::~RenderLayer()
{
    RemoveAll();
    delete tiles;
}

void RenderLayer::RemoveAll()
{
    tiles->Clear();

    if(!_objectCount)
        return;

    //logdebug("WARNING: ~RenderLayer(%u), sending %u objects to layer 0", _id, _objectCount);
    
    ASSERT(!_firstBlock->prev);
    for(Block *blk = _firstBlock; blk; )
    {
        RenderObject **roptr = blk->ptrs;
        for(unsigned int j = 0; j < BLOCK_SIZE; ++j)
        {
            if(RenderObject *ro = *roptr++)
            {
                ro->_layerBlock = NULL;
                ro->_layerPtr = NULL;

                //if(ro->getLayer())
                //    ro->toLayer(0); // this also erases from current layer
                //else
                    ro->kill();
            }
        }
        Block *rm = blk;
        blk = blk->next;
        XDELETE_NN(rm, Arenas::chunkAlloc);
    }
    _firstBlock = NULL;
    _lastBlock = NULL;
    _objectCount = 0;
}

RenderLayer::Block *RenderLayer::_AppendBlock()
{
    Block *blk = XNEW(Block, Arenas::chunkAlloc);
    memset(blk, 0, sizeof(Block));
    blk->direction = 1;

    blk->prev = _lastBlock;
    if(_lastBlock)
        _lastBlock->next = blk;
    _lastBlock = blk;
    if(!_firstBlock)
        _firstBlock = blk;
    return blk;
}

RenderLayer::Block *RenderLayer::_PrependBlock()
{
    Block *blk = XNEW(Block, Arenas::chunkAlloc);
    memset(blk, 0, sizeof(Block));
    blk->direction = -1;
    blk->wIdx = BLOCK_SIZE - 1;

    blk->next = _firstBlock;
    if(_firstBlock)
        _firstBlock->prev = blk;
    _firstBlock = blk;
    if(!_lastBlock)
        _lastBlock = blk;
    return blk;
}

void RenderLayer::_UnlinkBlock(Block *blk)
{
    ASSERT(blk->objCount == 0);
    // TODO: upper 2 ifs can be merged with lower 2 ??
    if(blk->prev)
    {
        blk->prev->next = blk->next;
    }
    else
    {
        ASSERT(blk == _firstBlock);
    }

    if(blk->next)
    {
        blk->next->prev = blk->prev;
    }
    else
    {
        ASSERT(blk == _lastBlock);
    }

    if(blk == _firstBlock)
    {
        ASSERT(blk->prev == NULL);
        _firstBlock = blk->next;
    }
    if(blk == _lastBlock)
    {
        ASSERT(blk->next == NULL);
        _lastBlock = blk->prev;
    }
    XDELETE_NN(blk, Arenas::chunkAlloc);
}

void RenderLayer::_AppendRO(RenderObject *ro)
{
    ASSERT(ro->_layerBlock == NULL);
    Block *blk = _lastBlock;
    if(!(blk && blk->hasSpace()))
        blk = _AppendBlock();
    blk->insert(ro);
    ++_objectCount;
}

void RenderLayer::_PrependRO(RenderObject *ro)
{
    ASSERT(ro->_layerBlock == NULL);
    Block *blk = _firstBlock;
    if(!(blk && blk->hasSpace()))
        blk = _PrependBlock();
    blk->insert(ro);
    ++_objectCount;
}

void RenderLayer::_UnlinkRO(RenderObject *ro)
{
    ASSERT(ro->_layerBlock);
    Block *blk = ro->_layerBlock;
    blk->remove(ro);
    if(!blk->objCount)
        _UnlinkBlock(blk);
    --_objectCount;
}

void RenderLayer::Add(RenderObject *ro)
{
    if(ro->getLayerPtr() || ro->_layerBlock)
        logerror("RenderLayer(%u)::Add: Object %p already has a layer set, may go haywire later!", _id, ro);
    ro->_layerPtr = this;

    _AppendRO(ro);
}

void RenderLayer::AddFront(RenderObject *ro)
{
    if(ro->getLayerPtr() || ro->_layerBlock)
        logerror("RenderLayer(%u)::AddFront: Object %p already has a layer set (%u), may go haywire later!", _id, ro,ro->getLayerPtr()->GetID());
    ro->_layerPtr = this;

    _PrependRO(ro);
}

void RenderLayer::Remove(RenderObject *ro)
{
    if(ro->getLayerPtr() != this)
        logerror("RenderLayer(%u)::Remove: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayerPtr()->GetID());
    ro->_layerPtr = NULL;

    _UnlinkRO(ro);
    ro->_indexInBlock = -1;
    ro->_layerBlock = NULL;
}

void RenderLayer::MoveToBack(RenderObject *ro)
{
    if(ro->getLayerPtr() != this)
        logerror("RenderLayer(%u)::MoveToBack: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayerPtr()->GetID());

    // rendered first -> in back
    _UnlinkRO(ro);
    _AppendRO(ro);
}

void RenderLayer::MoveToFront(RenderObject *ro)
{
    if(ro->getLayerPtr() != this)
        logerror("RenderLayer::MoveToFront: Object %p is on wrong layer (%u), may go haywire later!", _id, ro, ro->getLayerPtr()->GetID());

    // rendered last -> in front
    _UnlinkRO(ro);
    _PrependRO(ro);
}

void RenderLayer::Render()
{
    // If no objects exist, no blocks exist
    ASSERT(_objectCount || !(_firstBlock || _lastBlock));

    Renderer *r = engine->GetRenderer();
    bool noCull = false;
    
    if(isFixedPos())
    {
        r->setupScreenScale();
        noCull = true;
    }
    else
        r->setupRenderPositionAndScale();

    //tiles->onRender();
    r->renderObject(tiles);

    
    for(Block *blk = _firstBlock; blk; blk = blk->next)
    {
        RenderObject **roptr = blk->ptrs;
        for(unsigned int i = 0; i < BLOCK_SIZE; ++i)
        {
            if(RenderObject *ro = *roptr++)
            {
                if(!ro->getParent() && (noCull || ro->isOnScreen()))
                    r->renderObject(ro);
            }
        }
    }
}

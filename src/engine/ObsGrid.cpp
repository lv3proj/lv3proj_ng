#include "ObsGrid.h"
#include "Tile.h"
#include "TileGrid.h"
#include "Engine.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"

#define INCREMENTAL_OPTIMIZE_BLOCKS 3

ObsGrid::ObsGrid()
 : _memInUse(0)
 , _sparepos(0)
 , _full(NULL)
 , _empty(NULL)
 , _incrementalX(0)
 , _incrementalY(0)
{
}

ObsGrid::~ObsGrid()
{
    Clear();
}

void ObsGrid::Clear()
{
    for(size_t i = 0; i < _spare.size(); ++i)
        delete [] _spare[i];
    for(unsigned int y = 0; y < _grid.size1d(); ++y)
        for(unsigned int x = 0; x < _grid.size1d(); ++x)
        {
            mask *block = _grid(x, y);
            if(block != _empty && block != _full)
                delete [] block;
        }
    _grid.free();
    delete [] const_cast<mask*>(_full);
    delete [] const_cast<mask*>(_empty);
    _full = NULL;
    _empty = NULL;
}

void ObsGrid::Init(unsigned int gridsize, unsigned int blocksize, unsigned int keepspare)
{
    Clear();

    _keepspare = keepspare;
    _spare.resize(keepspare, (mask*)NULL);

    unsigned int t = 1;
    _blockshift = 0;
    while(t < blocksize)
    {
        t <<= 1;
        ++_blockshift;
    }
    _blockdim = t; // something like 0x..100
    _blockbits = t - 1; // -> 0x..0FF
    _blocksize = _blockdim * _blockdim;

    mask *full = new mask[_blocksize];
    mask *empty = new mask[_blocksize];
    memset(full, OBS_ANY, _blocksize * sizeof(mask));
    memset(empty, OBS_NONE, _blocksize * sizeof(mask));
    _full = full;
    _empty = empty;

    _grid.resize(gridsize, empty);
}

unsigned char ObsGrid::getObs(unsigned int x, unsigned int y)
{
    mask *block = _lookupBlock(x, y);

    /*if(block == _full)
        return OBS_ANY;
    if(block == _empty)
        return OBS_NONE;*/

    // same effect, but branchless

    x &= _blockbits;
    y &= _blockbits;

    return block[(y << _blockshift) | x];
}

void ObsGrid::_dropBlock(mask *block)
{
    // special; these can't be dropped
    if(block == _empty || block == _full)
        return;

    if(_sparepos >= _keepspare)
    {
        delete [] block;
        _memInUse -= _blocksize;
        return;
    }

    _spare[_sparepos++] = block;
}

ObsGrid::mask *ObsGrid::_newBlock()
{
    if(!_sparepos)
    {
        _memInUse += _blocksize;
        return new mask[_blocksize];
    }

    --_sparepos;
    mask *ret = _spare[_sparepos];
    _spare[_sparepos] = NULL;
    return ret;
}

void ObsGrid::setObs(unsigned int x, unsigned int y, ObsType obs)
{
    mask *block = _lookupBlock(x, y);
    if(block == _full)
    {
        block = _newBlock();
        memset(block, OBS_ANY, _blocksize * sizeof(mask));
    }
    else if(block == _empty)
    {
        block = _newBlock();
        memset(block, OBS_NONE, _blocksize * sizeof(mask));
    }

    x &= _blockbits;
    y &= _blockbits;


    block[(y << _blockshift) | x] = obs;
}

void ObsGrid::OptimizeAll()
{
    unsigned int optimized = 0;
    unsigned int membefore = _memInUse;
    const unsigned int dim = _grid.size1d();
    for(unsigned int y = 0; y < dim; ++y)
        for(unsigned int x = 0; x < dim; ++x)
            optimized += _optimizeBlock(x, y);
    _incrementalX = 0;
    _incrementalY = 0;
    logdev("ObsGrid::OptimizeAll(): %u blocks crunched (mem before: %u, after: %u)", optimized, membefore, _memInUse);
}

void ObsGrid::OptimizeIncremental()
{
    const unsigned int dim = _grid.size1d();
    if(!dim)
        return;
    for(unsigned int i = 0; i < INCREMENTAL_OPTIMIZE_BLOCKS; ++i)
    {
        if(_incrementalX >= dim)
        {
            _incrementalX = 0;
            ++_incrementalY;
            if(_incrementalY >= dim)
                _incrementalY = 0;
        }
        _optimizeBlock(_incrementalX++, _incrementalY);
    }
}

bool ObsGrid::_optimizeBlock(unsigned int x, unsigned int y)
{
    mask *block = _grid(x, y);
    if(block == _empty || block == _full)
        return false;

    unsigned int fre = OBS_NONE; // 0
    unsigned int solid = OBS_ANY; // 0xff
    const unsigned int bs = _blocksize;
    // TODO: this could benefit from full uint32-block comparisons
    for(unsigned int i = 0; i < bs; ++i)
    {
        fre |= block[i]; // any non-free or non-solid block will get the original values clobbered
        solid &= block[i];
    }
    if(fre == 0)
    {
        _dropBlock(block);
        _grid(x, y) = const_cast<mask*>(_empty);
        return true;
    }
    if(solid == OBS_ANY)
    {
        _dropBlock(block);
        _grid(x, y) = const_cast<mask*>(_full);
        return true;
    }
    return false;
}

void ObsGrid::Setup()
{
    const unsigned int dim = _grid.size1d();
    for(unsigned int y = 0; y < dim; ++y)
    {
        for(unsigned int x = 0; x < dim; ++x)
        {
            UpdateTile(x, y);
        }
    }

    //OptimizeAll();
}

void ObsGrid::UpdateTile(unsigned int x, unsigned int y)
{
    mask *block = _grid(x, y);
    if(block == _full)
        return;

    //unsigned int fre = OBS_NONE; // 0
    unsigned int lrcount = engine->layers->GetLayerCount();
    for(unsigned int lr = 1; lr < lrcount; ++lr)
    {
        RenderLayer *layer = engine->layers->GetLayer(lr);
        if(!layer)
            continue;
        if(!layer->tiles.colliding)
            continue;

        TileGrid& tg = layer->tiles;

        Tile *tile = tg.GetTile(x, y);
        if(!tile)
            continue;

        switch(tile->getTileObsType())
        {
            case TO_FULLFREE:
                continue;

            case TO_FULLSOLID:
                _dropBlock(block);
                _grid(x, y) = const_cast<mask*>(_full);
                return;

            case TO_MIXED:
            {
                const unsigned int tdim = _blockdim;
                if(block == _empty)
                {
                    block = _newBlock();
                    memset(block, OBS_NONE, _blocksize * sizeof(mask));
                    _grid(x, y) = block;
                }

                mask *ptr = block;
                unsigned int solid = OBS_ANY; // 0xff

                for(unsigned int ty = 0; ty < tdim; ++ty)
                {
                    for(unsigned int tx = 0; tx < tdim; ++tx)
                    {
                        mask t = tile->getObs(tx, ty) | *ptr;
                        *ptr++ = t;
                        solid &= t;
                        //fre |= t;
                    }
                }

                if(solid == OBS_ANY)
                {
                    _dropBlock(block);
                    _grid(x, y) = const_cast<mask*>(_full);
                    return;
                }
            }
        }
    }

    // THIS IS NOT SAFE
    /*if(fre == OBS_NONE)
    {
        _dropBlock(block);
        _grid(x, y) = const_cast<mask*>(_empty);
    }*/
}



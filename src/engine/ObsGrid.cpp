#include "ObsGrid.h"
#include "Tile.h"
#include "TileGrid.h"
#include "Engine.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"
#include "Arenas.h"

#include "collision/Collidable.h"
#include "collision/AABB.h"
#include "collision/Circle.h"
#include "collision/Line.h"

#define GRID_ALLOCP ((Arenas::ObsGridMem*)(this->_allocator))
#define GRID_ALLOC (*GRID_ALLOCP)

#define INCREMENTAL_OPTIMIZE_BLOCKS 3

ObsGrid::ObsGrid()
 : _full(NULL)
 , _empty(NULL)
 , _incrementalX(0)
 , _incrementalY(0)
 , _allocator(NULL)
{
}

ObsGrid::~ObsGrid()
{
    Clear();
}

void ObsGrid::Clear()
{
    if(!_allocator)
        return;

    for(unsigned int y = 0; y < _grid.size1d(); ++y)
        for(unsigned int x = 0; x < _grid.size1d(); ++x)
        {
            mask *block = _grid(x, y);
            if(block != _empty && block != _full)
                _dropBlock(block);
        }
    _grid.free();
    GRID_ALLOCP->Free(const_cast<mask*>(_full));
    GRID_ALLOCP->Free(const_cast<mask*>(_empty));
    _full = NULL;
    _empty = NULL;
    _incrementalX = 0;
    _incrementalY = 0;

    ASSERT(GetBlocksInUse() == 0);

    delete GRID_ALLOCP;
    _allocator = NULL;
}

void ObsGrid::Init(unsigned int gridsize, unsigned int blocksize)
{
    Clear();

    if(!(gridsize && blocksize))
        return;

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

    ASSERT(!_allocator);
    _allocator = new Arenas::ObsGridMem(Arenas::chunkAlloc, 256, _blocksize);


    if(!_full)
    {
        mask *full = _newBlock();
        memset(full, OBS_ANY, _blocksize * sizeof(mask));
        _full = full;
    }
    if(!_empty)
    {
        mask *empty = _newBlock();
        memset(empty, OBS_NONE, _blocksize * sizeof(mask));
        _empty = empty;
    }

    _grid.resize(gridsize, const_cast<mask*>(_empty));

    _width = _grid.size1d() * _blockdim;
    _height = _grid.size1d() * _blockdim;
}

void ObsGrid::_dropBlock(mask *block)
{
    // special; these can't be dropped
    if(block == _empty || block == _full)
        return;

    GRID_ALLOCP->Free(block);
}

ObsGrid::mask *ObsGrid::_newBlock()
{
    ASSERT(_allocator);
    return (mask*)GRID_ALLOCP->Allocate(_blocksize, 0, XMEM_SOURCE_INFO);
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
    unsigned int membefore = GetMemoryUsage();
    unsigned int optimized = 0;
    const unsigned int dim = _grid.size1d();
    for(unsigned int y = 0; y < dim; ++y)
        for(unsigned int x = 0; x < dim; ++x)
            optimized += _optimizeBlock(x, y);
    _incrementalX = 0;
    _incrementalY = 0;
    unsigned int memafter = GetMemoryUsage();
    logdev("ObsGrid::OptimizeAll(): %u blocks crunched (mem before: %u, after: %u)", optimized, membefore, memafter);
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

    OptimizeAll();
}

void ObsGrid::UpdateTile(unsigned int x, unsigned int y)
{
    if(x >= _grid.size1d() || y >= _grid.size1d())
        return;

    mask *block = _grid(x, y);

    _dropBlock(block);
    _grid(x, y) = block = const_cast<mask*>(_empty);

    unsigned int lrcount = engine->layers->GetLayerCount();
    for(unsigned int lr = 1; lr < lrcount; ++lr)
    {
        RenderLayer *layer = engine->layers->GetLayer(lr);
        if(!layer)
            continue;
        if(!layer->tiles->colliding)
            continue;

        TileGrid& tg = *(layer->tiles);

        Tile *tile = tg.GetTileSafe(x, y);
        if(!tile)
            continue;

        // FIXME: bits are not ORed together properly in case a TO_FULLSOLID appears.
        switch(tile->getTileObsType())
        {
            case TO_FULLSOLID:
                _dropBlock(block);
                _grid(x, y) = const_cast<mask*>(_full);
                return; // fully occupied, any other block above it will not affect the outcome anymore

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
                        *ptr++ |= t;
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
}


bool ObsGrid::collidesWith(const Collidable& c, Vector *result) const
{
    switch(c.getShape())
    {
        case COLL_AABB: return collideVsAABB((const AABB&)c, result);
        case COLL_CIRCLE: return collideVsCircle((const Circle&)c, result);
        case COLL_LINE: return collideVsLine((const Line&)c, result);

        default:
            assert(false);
    }
    return false;
}

bool ObsGrid::collideVsAABB(const AABB &c, Vector *v) const
{
    const Vector ul = c.upleft();
    const Vector dr = c.downright();
    int x1 = int(ul.x);
    int x2 = int(dr.x);
    int y1 = int(ul.y);
    int y2 = int(dr.y);
    if(x1 < 0)
        x1 = 0;
    if(x2 >= width())
        x2 = width()-1;
    if(y1 < 0)
        y1 = 0;
    if(y2 >= height())
        y2 = height()-1;

    // TODO: If this turns out to be a bottleneck,
    // do the comparisons blockwise, instead of
    // looking up each block multiple times for a single pixel.
    const unsigned char bits = c.collisionBits;
    for(int y = y1; y <= y2; ++y)
        for(int x = x1; x <= x2; ++x)
            if((getObs(x, y) & bits))
            {
                if(v)
                    *v = Vector(x, y);
                return true;
            }

    return false;
}

bool ObsGrid::collideVsCircle(const Circle &c, Vector *v) const
{
    const Vector pos = c.getPosition();
    int x1 = int(pos.x - c.radius);
    int x2 = int(pos.x + c.radius);
    int y1 = int(pos.y - c.radius);
    int y2 = int(pos.y + c.radius);
    if(x1 < 0)
        x1 = 0;
    if(x2 >= width())
        x2 = width()-1;
    if(y1 < 0)
        y1 = 0;
    if(y2 >= height())
        y2 = height()-1;

    int px = (int)pos.x;
    int py = (int)pos.y;
    int radiusSq = (int)(c.radius * c.radius);

    const unsigned char bits = c.collisionBits;
    for(int y = y1; y <= y2; ++y)
        for(int x = x1; x <= x2; ++x)
            if((getObs(x, y) & bits) /* && c.isPointInside(Vector(x, y))*/)
            {
                // this is faster than int->float conversion all the time as isPointInside() would do
                int cx = px - x;
                int cy = py - y;
                if(cx*cx + cy*cy <= radiusSq)
                {
                    if(v)
                        *v = Vector(x, y);
                    return true;
                }
            }

    return false;
}

struct GridTracer
{
    const ObsGrid &_g;
    const unsigned char _bits;
    inline GridTracer(const ObsGrid &g, unsigned char bits) : _g(g), _bits(bits) {}
    inline bool operator()(int x, int y) const
    {
        return x >= 0 && x < _g.width()
            && y >= 0 && y < _g.height()
            && (_g.getObs(x, y) & _bits);
    }
};

bool ObsGrid::collideVsLine(const Line& c, Vector *v) const
{
    GridTracer tr(*this, c.collisionBits);
    return c.tracei(tr, v, 1);
}

bool ObsGrid::getNormal(const Vector& pos, Vector& v, unsigned int resolution /* = 5 */,
                        unsigned int skip /* = 1 */, unsigned char bits /* = ~0 */) const
{
    struct
    {
        int x;
        int y;
    } points[256];

    int px = pos.x;
    int py = pos.y;
    int xmin = px - resolution;
    int xmax = px + resolution;
    int ymin = py - resolution;
    int ymax = py + resolution;

    if(xmin < 0)
        xmin = 0;
    if(xmax >= width())
        xmax = width()-1;
    if(ymin < 0)
        ymin = 0;
    if(ymax >= height())
        ymax = height()-1;

    unsigned int pointsFound = 0;
    int sz = resolution * resolution;

    for(int y = ymin; y <= ymax; y += skip)
        for(int x = xmin; x <= xmax; x += skip)
        {
            if(x == px && y == py)
                continue;
            if((getObs(x, y) & bits))
            {
                int dx = px - x;
                int dy = py - y;
                if(dx*dx + dy*dy <= sz)
                {
                    points[pointsFound].x = dx;
                    points[pointsFound++].y = dy;
                    if(pointsFound >= 256)
                    {
                        logerror("Warning: ObsGrid::getNormal() position buffer full");
                        goto foundEnough;
                    }
                }
            }
        }

    if(!pointsFound)
    {
        v.x = v.y = 0;
        return false;
    }

foundEnough:

    int allx = 0;
    int ally = 0;
    for(unsigned int i = 0; i < pointsFound; ++i)
    {
        allx += points[i].x;
        ally += points[i].y;
    }

    Vector normal(allx, ally);
    normal.normalize2D();

    v = normal;
    return true;
}

size_t ObsGrid::GetMemoryUsage() const
{
    return GRID_ALLOCP ? GRID_ALLOCP->GetTracker().GetActiveBytes() : 0;
}

size_t ObsGrid::GetBlocksInUse() const
{
    return GRID_ALLOCP ? GRID_ALLOCP->GetTracker().GetActiveAllocations() : 0;
}


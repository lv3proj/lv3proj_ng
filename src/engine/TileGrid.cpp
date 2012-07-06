#include "TileGrid.h"
#include "Engine.h"
#include "Renderer.h"
#include "ResourceMgr.h"

#include "Texture.h"

TileGrid::TileGrid()
: _used(0), _tileSize(0), colliding(false)
{
}

TileGrid::~TileGrid()
{
    Clear();
}

void TileGrid::Clear()
{
    const unsigned int dim = _tiles.size1d();
    for(unsigned int y = 0; y < dim; ++y)
        for(unsigned int x = 0; x < dim; ++x)
            SetTile(x, y, NULL);
}

void TileGrid::SetSize(unsigned int dim)
{
    // enlarging is easy as no tiles will disappear
    unsigned int oldsize = _tiles.size1d();
    unsigned int newsize = clp2(dim); // always n^2
    if(newsize >= oldsize)
    {
        _tiles.resize(dim, NULL);
        return;
    }

    // if shrinking the map, the tiles that are going to disappear have to be reference counted down properly
    // example:
    /* XXXX.....
    *  XXXX.....
    *  XXXX.....
    *  .........
    *  .........*/
    unsigned int y = 0, x;
    while(y < newsize)
    {
        for(x = newsize; x < oldsize; ++x)
            if(Tile *tile = _tiles(x,y))
                SetTile(x, y, NULL); // drop tile
        ++y;
    }
    for(y = newsize; y < oldsize; ++y)
        for(x = 0; x < oldsize; x++)
            if(Tile *tile = _tiles(x,y))
                SetTile(x, y, NULL); // drop tile

    _tiles.resize(newsize, NULL);
}

void TileGrid::SetTileByName(unsigned int x, unsigned int y, const char *name)
{
    if(name && *name)
    {
        Tile *tile = resMgr.LoadTile(name);
        if(!tile)
            return;

        SetTile(x, y, tile);
        tile->decref();
        return;
    }

    SetTile(x, y, NULL);
}

// Puts a tile to location (x,y). Set tile to NULL to remove current tile. Does ref-counting.
void TileGrid::SetTile(unsigned int x, unsigned int y, Tile *tile)
{
    if(x >= _tiles.size1d() || y >= _tiles.size1d())
    {
        logerror("Tried to set tile out of bounds!");
        return;
    }

    Tile *& tileref = _tiles(x,y);
    if(tileref == tile)
        return;

    // update amount of used tiles and drop the old one out of the tilemap, if required
    if(tileref)
    {
        tileref->decref(); // will be overwritten, decr ref
        if(!tile)
            --_used; // we have a tile currently, but setting NULL -> one tile less used
    }
    else if(tile) // tileref is NULL, but we are setting a tile -> one more tile used
    {
        ++_used;
    }

    if(tile) // we will use this, incr ref
    {
        tile->incref();

        if(!_tileSize)
            _tileSize = tile->getTexture()->getWidth();
    }
    else if(!_used)
        _tileSize = 0;

    // put tile in place
    tileref = tile;
}

void TileGrid::onRender() const
{
    if(!_used)
        return;

    Renderer *render = engine->GetRenderer();

    /*
    for(unsigned int y = 0; y < dim; ++y)
        for(unsigned int x = 0; x < dim; ++x)
            if(Tile *tile = _tiles(x, y))
                render->renderSingleTexture(tile->getTexture(), Vector(x * _tileSize, y * _tileSize));
    */

    int xstart, ystart, xend, yend;
    CalcRenderLimits(xstart, ystart, xend, yend);
    int dim = xend - xstart;
    if(dim <= 0 || (yend - ystart) <= 0)
        return;

    Vector start(_tileSize * xstart, _tileSize * ystart);
    const Vector step(_tileSize, 0);
    Texture **texarray = (Texture**)alloca(dim * sizeof(Texture*));
    for(int y = ystart; y < yend; ++y)
    {
        Tile * const * tileptr = &_tiles(0, y);
        for(int x = xstart; x < xend; ++x)
        {
            Tile *tile = tileptr[x];
            texarray[x-xstart] = tile ? tile->getTexture() : NULL;
        }
        render->renderTextureArray(texarray, dim, start, step);
        start.y += _tileSize;
    }
}

void TileGrid::CalcRenderLimits(int& x, int& y, int& x2, int& y2) const
{
    engine->CalcRenderLimits(_tiles.size1d(), _tileSize, x, y, x2, y2);
}


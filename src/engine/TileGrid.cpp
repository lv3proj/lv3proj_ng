#include "TileGrid.h"
#include "Engine.h"
#include "Renderer.h"
#include "ResourceMgr.h"

#include "Texture.h"

TileGrid::TileGrid()
: _used(0), _tileSize(0)
{
}

TileGrid::~TileGrid()
{
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

bool TileGrid::SetTileByName(unsigned int x, unsigned int y, const char *name)
{
    Tile *tile = resMgr.LoadTile(name);
    if(!tile)
        return false;

    SetTile(x, y, tile);
    tile->decref();
    return true;
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

        // HACK: move this somewhere else
        if(!_tileSize)
            _tileSize = tile->getTexture()->getWidth();
    }

    // put tile in place
    tileref = tile;
}

void TileGrid::onRender()
{
    if(!_used)
        return;

    Renderer *render = engine->GetRenderer();

    const unsigned int dim = _tiles.size1d();

    /*
    for(unsigned int y = 0; y < dim; ++y)
        for(unsigned int x = 0; x < dim; ++x)
            if(Tile *tile = _tiles(x, y))
                render->renderSingleTexture(tile->getTexture(), Vector(x * _tileSize, y * _tileSize));
    */

    Vector start;
    const Vector step(_tileSize, 0);
    Texture **texarray = (Texture**)alloca(dim * sizeof(Texture*));
    for(unsigned int y = 0; y < dim; ++y)
    {
        Tile **tileptr = &_tiles(0, y);
        for(unsigned int x = 0; x < dim; ++x)
        {
            Tile *tile = tileptr[x];
            texarray[x] = tile ? tile->getTexture() : NULL;
        }
        render->renderTextureArray(texarray, dim, start, step);
        start.y += _tileSize;
    }
}


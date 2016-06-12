#include "TileMgr.h"
#include "Tile.h"


TileMgr::TileMgr()
: _holes(0)
{
    _tiles.reserve(256);
}

TileMgr::~TileMgr()
{
    DeleteAll();
}

void TileMgr::DeleteAll()
{
    for(size_t i = 0; i < _tiles.size(); ++i)
        delete _tiles[i];
    _tiles.clear();
    _holes = 0;
}

Tile *TileMgr::CreateTile(Texture *tex, const Rect& rect)
{
    Tile *t = new Tile(tex, rect);
    if(_holes)
        for(size_t i = 0; i < _tiles.size(); ++i)
            if(!_tiles[i])
            {
                _tiles[i] = t;
                t->idx = i;
                --_holes;
               return t;
            }
    _tiles.push_back(t);
    t->idx = unsigned(_tiles.size() - 1);
    return  t;
}

void TileMgr::DeleteUnused()
{
    unsigned newholes = 0;
    for(size_t i = 0; i < _tiles.size(); ++i)
        if(Tile *t = _tiles[i])
            if(!t->refcount)
            {
                delete t;
                _tiles[i] = NULL;
                ++newholes;
            }
    _holes += newholes;
}

void TileMgr::DeleteTile(Tile *t)
{
    ASSERT(_tiles[t->idx] == t);
    _tiles[t->idx] = NULL;
    ++_holes;
    delete t;
}



#ifndef TILE_MGR_H
#define TILE_MGR_H

#include <vector>
#include "common.h"

class Tile;
class Texture;


class TileMgr
{
public:
    TileMgr();
    ~TileMgr();
    inline Tile *GetTile(unsigned idx) { return _tiles[idx]; }
    Tile *CreateTile(Texture *tex, const Rect& rect);
    void DeleteTile(Tile *);
    void DeleteAll();
    void DeleteUnused();
    unsigned GetNumTiles() const { return (unsigned)_tiles.size() - _holes; }


private:
    std::vector<Tile*> _tiles;
    unsigned _holes;
};


#endif


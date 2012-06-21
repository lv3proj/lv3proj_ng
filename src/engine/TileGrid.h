#ifndef TILEGRID_H
#define TILEGRID_H

#include "RenderObject.h"
#include "Tile.h"

class TileGrid : public RenderObject
{
public:
    TileGrid();
    virtual ~TileGrid();

    void SetSize(unsigned int dim);
    void SetTile(unsigned int x, unsigned int y, Tile *tile);
    bool SetTileByName(unsigned int x, unsigned int y, const char *name);

    virtual void onRender();

protected:

    array2d<Tile*> _tiles;
    unsigned int _used;
    float _tileSize;
};

#endif

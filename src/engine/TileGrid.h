#ifndef TILEGRID_H
#define TILEGRID_H

#include "RenderObject.h"
#include "Tile.h"

class TileGrid : public RenderObject
{
public:
    TileGrid();
    virtual ~TileGrid();
    void Clear();

    void SetSize(unsigned int dim);
    void SetTile(unsigned int x, unsigned int y, Tile *tile);

    inline unsigned int GetSize() const { return _tiles.size1d(); }
    inline Tile *GetTile(unsigned int x, unsigned int y) const { return _tiles(x, y); }
    inline Tile *GetTileSafe(unsigned int x, unsigned int y) const { return x < _tiles.size1d() && y < _tiles.size1d() ? _tiles(x, y) : NULL; }
    inline float GetTileSizef() const { return _tileSize; }

    virtual void onRender() const;

    bool colliding;

protected:

    array2d<Tile*> _tiles;
    unsigned int _used;
    float _tileSize;
};

#endif

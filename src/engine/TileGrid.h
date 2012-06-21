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

    inline unsigned int GetSize() const { return _tiles.size1d(); }
    inline Tile *GetTile(unsigned int x, unsigned int y) const { return _tiles(x, y); }
    inline float GetTileSizef() const { return _tileSize; }

    virtual void onRender();

    void CalcRenderLimits(int& x, int& y, int& x2, int& y2);

protected:

    array2d<Tile*> _tiles;
    unsigned int _used;
    float _tileSize;
};

#endif

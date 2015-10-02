#ifndef TILE_H
#define TILE_H

#include <algorithm>

class Texture;

#include "Vector.h"
#include "array2d.h"
#include "Resource.h"
#include "ObsGrid.h"

class TileMgr;
class TileGrid;
class Texture;

enum TileObsType
{
    TO_FULLFREE = 0,        // all pixels are free
    TO_MIXED,               // some pixels are obstructed
    TO_FULLSOLID,           // all pixels are obstructed
};

// A Tile is a texture with collision/obstruction information.

class Tile
{
    friend class TileMgr;
    friend class TileGrid;

public:
    Tile(Texture *tex, const Rect& r);
    ~Tile();

    inline Texture *getTexture() { return _tex.content(); }

    bool CalcCollision(); // sets up collision mask

    inline TileObsType getTileObsType() const { return _tileobs; }

    inline unsigned char isObs(unsigned int x, unsigned int y, ObsType obs = OBS_ANY) const
    {
        return _mask(x, y) & obs;
    }

    inline unsigned char getObs(unsigned int x, unsigned int y) const
    {
        return _mask(x, y);
    }

    inline const array2d<unsigned char>& getObsMask() const { return _mask; }
    inline unsigned int getSize() const { return _mask.size1d(); }
    inline unsigned getIdx() const { return idx; }

    inline const UV& getULTC() const { return upperLeftTextureCoords; }
    inline const UV& getLRTC() const { return lowerRightTextureCoords; }

    void setTexture(Texture *tex);
    void setRect(Rect r);

    const Rect& getRect() const { return rect; }

protected:

    TileObsType _tileobs;
    array2d<unsigned char> _mask;
    CountedPtr<Texture> _tex;
    UV upperLeftTextureCoords;
    UV lowerRightTextureCoords;
    Rect rect;
    unsigned idx;// index in store
    unsigned refcount;
};

#endif

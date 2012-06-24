#ifndef TILE_H
#define TILE_H

class Texture;

#include "array2d.h"
#include "Resource.h"
#include "ObsGrid.h"

enum TileObsType
{
    TO_FULLFREE = 0,
    TO_FULLSOLID,           // all pixels are obstructed
    //TO_FULLSOLID_OPAQUE,    // all pixels are fully obstructed and fully opaque (i.e. underlying tiles do not need to be rendered.)
    TO_MIXED,
};

// A Tile is a texture with collision/obstruction information.


class Tile : public Resource
{
public:
    Tile(Texture *tex);
    virtual ~Tile();

    virtual unsigned int usedMem() const { return _mask.size2d(); }

    inline Texture *getTexture() const { return _tex; }

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

protected:

    Texture *_tex;
    TileObsType _tileobs;
    array2d<unsigned char> _mask;

};

#endif

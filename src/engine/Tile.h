#ifndef TILE_H
#define TILE_H

class Texture;

#include "array2d.h"
#include "Resource.h"

// bitmask
enum ObsType
{
    OBS_NONE = 0x00,
    OBS_WALL = 0x01,
};

// A Tile is a texture with collision/obstruction information.


class Tile : public Resource
{
public:
    Tile(Texture *tex);
    virtual ~Tile();

    inline Texture *getTexture() const { return _tex; }

    bool CalcCollision(); // sets up collision mask

protected:

    Texture *_tex;
    array2d<unsigned char> _mask;

};

#endif

#ifndef OBSGRID_H
#define OBSGRID_H

#include "common.h"
#include <vector>
#include <array2d.h>

class Collidable;
class Circle;
class AABB;
class Line;
class Vector;


// bitmask
enum ObsType
{
    OBS_NONE = 0x00,
    OBS_WALL = 0x01,

    OBS_ANY = 0xff,
};

// FIXME: Allow any grid size, i.e. in blocks of 4, 10, 20, ... pixels
// instead of only pixel-perfect resolution
// This will be useful to save memory and to speed things up
// if high precision isn't required.

class ObsGrid
{
    friend class ObsGridRender;
public:

    typedef unsigned char mask;

    ObsGrid();
    ~ObsGrid();

    // gridsize: dimension of grid (how many blocks per axis)
    // blocksize:dimension of each block, ie. 16x16
    void Init(unsigned int gridsize, unsigned int blocksize, unsigned int keepspare = 30);
    void Setup();
    void UpdateTile(unsigned int x, unsigned int y);
    // TODO: update function not involving tiles?

    void Clear();
    void OptimizeAll();
    void OptimizeIncremental();
    unsigned int GetMemoryUsage() const { return _memInUse; }

    inline unsigned char getObs(unsigned int x, unsigned int y) const;
    void setObs(unsigned int x, unsigned int y, ObsType obs);

    inline bool collidesWith(const Collidable *c, Vector *result = NULL) const { return c && collidesWith(*c, result); }
    bool collidesWith(const Collidable& c, Vector *result) const;
    bool collideVsAABB(const AABB& c, Vector *result) const;
    bool collideVsCircle(const Circle& c, Vector *result) const;
    bool collideVsLine(const Line& c, Vector *result) const;

    bool getNormal(const Vector& pos, Vector& result, unsigned int resolution = 5, unsigned int skip = 1, unsigned char bits = ~0) const;

    inline int width() const { return _width; }
    inline int height() const { return _height; }

private:

    inline mask *_lookupBlock(unsigned int x, unsigned int y) const;
    void _dropBlock(mask*);
    mask *_newBlock();
    bool _optimizeBlock(unsigned int x, unsigned int y);

    array2d<mask*> _grid;

    int _width; // blocksize * blockdim => the max. values for getObs()
    int _height;

    unsigned int _blocksize; // in bytes
    unsigned int _blockdim; // _blockdim*_blockdim == _blocksize
    unsigned int _blockshift;
    unsigned int _blockbits; // == _blocksize - 1, use to mask out higher bits from coordinates

    const mask *_full;
    const mask *_empty;

    std::vector<mask*> _spare; // unused
    unsigned int _sparepos; // always points to last insert position; i.e. _spare is empty if this is 0
    unsigned int _keepspare;

    unsigned int _incrementalX;
    unsigned int _incrementalY;


    unsigned int _memInUse;
};


ObsGrid::mask *ObsGrid::_lookupBlock(unsigned int x, unsigned int y) const
{
    const unsigned int sh = _blockshift;
    const unsigned int bx = x >> sh;
    const unsigned int by = y >> sh;
    return _grid(bx, by);
}

unsigned char ObsGrid::getObs(unsigned int x, unsigned int y) const
{
    mask *block = _lookupBlock(x, y);

    x &= _blockbits;
    y &= _blockbits;

    return block[(y << _blockshift) | x];
}



#endif


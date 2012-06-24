#ifndef OBSGRID_H
#define OBSGRID_H

#include <vector>
#include <array2d.h>


// bitmask
enum ObsType
{
    OBS_NONE = 0x00,
    OBS_WALL = 0x01,

    OBS_ANY = 0xff,
};

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

    unsigned char getObs(unsigned int x, unsigned int y);
    void setObs(unsigned int x, unsigned int y, ObsType obs);

private:

    inline mask *_lookupBlock(unsigned int x, unsigned int y);
    void _dropBlock(mask*);
    mask *_newBlock();
    bool _optimizeBlock(unsigned int x, unsigned int y);

    array2d<mask*> _grid;

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


ObsGrid::mask *ObsGrid::_lookupBlock(unsigned int x, unsigned int y)
{
    unsigned int sh = _blockshift;
    unsigned int bx = x >> sh;
    unsigned int by = y >> sh;
    return _grid(bx, by);
}



#endif


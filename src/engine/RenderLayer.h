#ifndef RENDERLAYER_H
#define RENDERLAYER_H

#include <list>
#include <string>
#include "Vector.h"

class RenderObject;
class TileGrid;
class Renderer;

class RenderLayer
{
    friend class RenderLayerMgr;

public:

    static const unsigned int BLOCK_SIZE = 128;

    struct Block
    {
        Block *next;
        Block *prev;
        unsigned int objCount;
        unsigned int wIdx; // Next index to write to (possibly out of range) [intentionally unsigned, uses wrap in hasSpace()]
        int direction; // either +1 or -1 (append or prepend)
        RenderObject *ptrs[BLOCK_SIZE];

        inline bool hasSpace() const { return wIdx < BLOCK_SIZE; }

        void insert(RenderObject *ro);
        void remove(RenderObject *ro);
    };

public:

    RenderLayer(unsigned int id);
    ~RenderLayer();

    void Add(RenderObject *);
    void AddFront(RenderObject *);
    void Remove(RenderObject *);
    void RemoveAll();
    void Render(Renderer *r);
    void MoveToFront(RenderObject *);
    void MoveToBack(RenderObject *);
    inline unsigned int GetID() const { return _id; }
    inline bool isFixedPos() const { return parallax.isZero(); }

    TileGrid *tiles;

    Vector parallax;
    bool visible;

protected:

    Block *_AppendBlock();
    Block *_PrependBlock();
    void _UnlinkBlock(Block *);
    void _AppendRO(RenderObject *);
    void _PrependRO(RenderObject *);
    void _UnlinkRO(RenderObject *);

    void _RenderCull(Renderer *r);
    void _RenderNoCull(Renderer *r);

    unsigned int _id; // position in vector in RenderLayerMgr
    //std::list<RenderObject*> _objs;

    unsigned int _objectCount;
    Block *_firstBlock;
    Block *_lastBlock;


};


#endif

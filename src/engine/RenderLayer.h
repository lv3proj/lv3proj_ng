#ifndef RENDERLAYER_H
#define RENDERLAYER_H

#include <list>
#include <string>
#include "TileGrid.h"

class RenderObject;


class RenderLayer
{
    friend class RenderLayerMgr;

public:
    RenderLayer();
    ~RenderLayer();

    void Add(RenderObject *);
    void AddFront(RenderObject *);
    void Remove(RenderObject *);
    void RemoveAll();
    void Render();
    void MoveToFront(RenderObject *);
    void MoveToBack(RenderObject *);
    inline unsigned int GetID() const { return _id; }

    TileGrid tiles;

    std::string name;
    bool noCamera; // FIXME: want to do this somehow else

protected:
    unsigned int _id; // position in vector in RenderLayerMgr
    std::list<RenderObject*> _objs;

};


#endif

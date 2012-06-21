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

    TileGrid tiles;

    std::string name;

protected:
    unsigned int _id; // position in vector in RenderLayerMgr
    std::list<RenderObject*> _objs;

};


#endif

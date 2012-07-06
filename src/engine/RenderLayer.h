#ifndef RENDERLAYER_H
#define RENDERLAYER_H

#include <list>
#include <string>
#include "Vector.h"

class RenderObject;
class TileGrid;

class RenderLayer
{
    friend class RenderLayerMgr;

public:
    RenderLayer(unsigned int id);
    ~RenderLayer();

    void Add(RenderObject *);
    void AddFront(RenderObject *);
    void Remove(RenderObject *);
    void RemoveAll();
    void Render();
    void MoveToFront(RenderObject *);
    void MoveToBack(RenderObject *);
    inline unsigned int GetID() const { return _id; }

    TileGrid *tiles;

    std::string name;
    Vector parallax;

protected:
    unsigned int _id; // position in vector in RenderLayerMgr
    std::list<RenderObject*> _objs;

};


#endif

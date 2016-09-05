#ifndef TILEMAPRENDER_H
#define TILEMAPRENDER_H

#include "Objects.h"
#include "TileMap.h"

class TileMapRender : public VirtualRenderObject
{
public:
    TileMapRender() {}
    virtual ~TileMapRender() {}
    virtual void render() const;

    CountedPtr<TileMap> tilemap;
};

#endif

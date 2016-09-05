#ifndef TILEMAP_H
#define TILEMAP_H

#include "VertexBuffer.h"
#include "Texture.h"
#include "array2d.h"
#include "SpriteSheet.h"

class SpriteSheet;

class TileMap : public Refcounted
{
    friend class TileMapRender;
public:
    typedef unsigned short u16;
    TileMap();
    ~TileMap();

    void resize(unsigned w, unsigned h);
    CountedPtr<SpriteSheet> spritesheet;
    void updateRenderData();

    inline u16& operator()(unsigned x, unsigned y) { return _map(x, y); }
    inline const u16& operator()(unsigned x, unsigned y) const { return _map(x, y); }

    struct RenderData
    {
        VertexBuffer ibo;
        CountedPtr<const Texture> tex;
    };

private:
    FastArray2D<u16> _map;

    std::vector<RenderData> rdata;
    VertexBuffer vbo;
};



#endif

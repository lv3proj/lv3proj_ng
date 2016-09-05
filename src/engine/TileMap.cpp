#include "TileMap.h"
#include <assert.h>
#include "SpriteSheet.h"

TileMap::TileMap()
: spritesheet(0)
{
}

TileMap::~TileMap()
{
}

void TileMap::resize(unsigned w, unsigned h)
{
    _map.resize(w, h);
}

void TileMap::updateRenderData()
{
    assert(spritesheet);

    struct V
    {
        inline V(float x_, float y_, float u_, float v_)
            : x(x_), y(y_), u(u_), v(v_) {}
        float x, y, u, v;
    };

    struct Tri
    {
        inline Tri(unsigned a_, unsigned b_, unsigned c_)
            : a(a_), b(b_), c(c_) {}
        unsigned a, b, c;
    };

    struct Tris
    {
        std::vector<Tri> tri;
        const Texture *tex;
    };

    std::vector<Tris> idx;
    std::vector<V> v;

    const unsigned w = _map.width();
    const unsigned h = _map.height();
    for(unsigned y = 0; y < h; ++y)
        for(unsigned x = 0; x < w; ++x)
        {
            const SpriteSheet::Def& d = spritesheet->getDef(_map(x, y));
            if(d.texIdx >= v.size())
                idx.resize(d.texIdx + 1);
            Tris& tt = idx[d.texIdx];
            std::vector<Tri>& ii = tt.tri;
            tt.tex = &spritesheet->getTex(d);

            const float x1 = float(x);
            const float x2 = float(x+1);
            const float y1 = float(y);
            const float y2 = float(y+1);

            unsigned i = unsigned(v.size());
            v.push_back(V(x1, y1, d.u1, d.v1));
            v.push_back(V(x2, y1, d.u2, d.v1));
            v.push_back(V(x1, y2, d.u1, d.v2));
            v.push_back(V(x2, y2, d.u2, d.v2));

            ii.push_back(Tri(i, i+1, i+2));
            ii.push_back(Tri(i+2, i+1, i+3));
        }


    vbo.alloc(&v[0], v.size() * sizeof(V));
    vbo.used = v.size();

    rdata.resize(idx.size());
    for(size_t i = 0; i < idx.size(); ++i)
    {
        rdata[i].tex = idx[i].tex;
        rdata[i].ibo.alloc(&idx[i].tri[0], idx[i].tri.size() * sizeof(Tri));
        rdata[i].ibo.used = unsigned(idx[i].tri.size());
    }
}

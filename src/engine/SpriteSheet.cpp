#include "SpriteSheet.h"
#include "Engine.h"
#include "ResourceMgr.h"


SpriteSheet::SpriteSheet()
{
}

SpriteSheet::~SpriteSheet()
{
}

int SpriteSheet::addTexAutoTile(const char *name, unsigned sx, unsigned sy)
{
    Texture *tex = g_engine->resmgr->getTex(name);
    if(!tex)
        return -1;

    texs.push_back(tex);
    Def d;
    d.w = sx;
    d.h = sy;
    d.texIdx = unsigned(texs.size() - 1);
    const unsigned w = tex->getWidth();
    const unsigned h = tex->getHeight();
    const float wf = (float)w;
    const float hf = (float)h;
    int c = 0;
    for(unsigned y = 0; h < w; y += sy)
        for(unsigned x = 0; x < w; x += sx)
        {
            d.x = x;
            d.y = y;
            d.u1 = float(x) / wf;
            d.v1 = float(y) / hf;
            d.u2 = float(x+sx) / wf;
            d.v2 = float(y+sy) / hf;
            defs.push_back(d);
            ++c;
        }
    return c;
}


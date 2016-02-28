#include "common.h"
#include "Tile.h"
#include "Texture.h"
#include "ResourceMgr.h"
#include "io/image.h"

Tile::Tile(Texture *tex, const Rect& r)
: _tileobs(TO_FULLFREE), _tex(NULL), rect(r), refcount(0)
{
    setTexture(tex);
}

Tile::~Tile()
{
}

void Tile::setTexture(Texture *tex)
{
    if(tex == _tex.content())
        return;
    _tex = tex;
    setRect(rect);
}

void Tile::setRect(Rect r)
{
    if(r.isZero())
    {
        r.w = _tex->getWidth();
        r.h = _tex->getHeight();
    }
    rect = r;
    CalcCollision();
    const float tw = float(_tex->getWidth());
    const float th = float(_tex->getHeight());
    upperLeftTextureCoords  = UV(r.x / tw, r.y / th);
    lowerRightTextureCoords = UV((r.x + r.w) / tw, (r.y + r.h) / th);
}

void Tile::CalcCollision()
{
    const Image *img = _tex->getSourceImage();

    _mask.resize(std::max(rect.w, rect.h), 0);

    unsigned int fre = 0, solid = 0;

    for(unsigned int y = 0; y < rect.h; ++y)
    {
        for(unsigned int x = 0; x < rect.w; ++x)
        {
            Image::Pixel pix = (*img)(rect.x + x, rect.y + y);

            // TODO: max. threshold?
            if(pix.a)
            {
                _mask(x, y) |= OBS_ANY; //OBS_WALL;
                ++solid;
            }
            else
            {
                _mask(x, y) &= ~OBS_ANY; // OBS_WALL;
                ++fre;
            }
        }
    }

    if(!solid)
        _tileobs = TO_FULLFREE;
    else if(!fre)
        _tileobs = TO_FULLSOLID;
    else
        _tileobs = TO_MIXED;
}

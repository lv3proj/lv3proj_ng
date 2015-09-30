#include "common.h"
#include "Tile.h"
#include "Texture.h"
#include "SDLSurfaceResource.h"
#include "SDL_func.h"
#include "ResourceMgr.h"

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

bool Tile::CalcCollision()
{
    SDLSurfaceResource *res = resMgr.LoadImg(_tex->name());
    if(!res)
    {
        logerror("Tile::CalcCollision(): No image for %s", _tex->name());
        return false;
    }

    SDL_Surface *surf = res->getSurface();
    SDL_PixelFormat *fmt = surf->format;

    _mask.resize(std::max(rect.w, rect.h), 0);

    unsigned int fre = 0, solid = 0;

    for(unsigned int y = 0; y < rect.h; ++y)
    {
        for(unsigned int x = 0; x < rect.w; ++x)
        {
            unsigned pix = SDLfunc_getpixel(surf, rect.x + x, rect.y + y);
            unsigned a = pix >> 24;

            // TODO: max. threshold?
            if(a)
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

    return true;
}

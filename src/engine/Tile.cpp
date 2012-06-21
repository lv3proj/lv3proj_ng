#include "common.h"
#include "Tile.h"
#include "Texture.h"
#include "SDLSurfaceResource.h"
#include "SDL_func.h"
#include "ResourceMgr.h"

Tile::Tile(Texture *tex)
: Resource(tex->name(), RESOURCE_TILE)
, _tex(tex)
{
    tex->incref();
}

Tile::~Tile()
{
    _tex->decref();
}

bool Tile::CalcCollision()
{
    SDLSurfaceResource *res = resMgr.LoadImg(name());
    if(!res)
    {
        logerror("Tile::CalcCollision(): No image for %s", name());
        return false;
    }

    unsigned int pix;
    unsigned char r, g, b, a;
    SDL_Surface *surf = res->getSurface();
    SDL_PixelFormat *fmt = surf->format;
    unsigned int w = surf->w, h = surf->h;

    _mask.resize(0, 0);

    // TODO: speed this up. This is really slow.
    for(unsigned int y = 0; y < h; ++y)
    {
        for(unsigned int x = 0; x < w; ++x)
        {
            pix = SDLfunc_getpixel(surf, x, y);
            SDL_GetRGBA(pix, fmt, &r, &g, &b, &a);

            // TODO: max. threshold?
            if(a)
                _mask(x, y) |= OBS_WALL;
            else
                _mask(x, y) &= ~OBS_WALL;
        }
    }

    res->decref();

    return true;
}

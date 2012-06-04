#ifndef SDL_SURFACE_RESOURCE_H
#define SDL_SURFACE_RESOURCE_H

#include "Resource.h"

struct SDL_Surface;


class SDLSurfaceResource : public Resource
{
public:
    SDLSurfaceResource(const char *name, SDL_Surface *s);

    inline SDL_Surface *getSurface() const { return _surface; }

    virtual unsigned int usedMem() const;

protected:
    ~SDLSurfaceResource();

    SDL_Surface *_surface;
};

#endif

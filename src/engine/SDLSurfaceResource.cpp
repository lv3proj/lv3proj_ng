#include "SDLSurfaceResource.h"
#include <SDL/SDL.h>

SDLSurfaceResource::SDLSurfaceResource(const char *name, SDL_Surface *s)
 : Resource(name, RESOURCE_IMAGE)
 , _surface(s)
{
}

SDLSurfaceResource::~SDLSurfaceResource()
{
    SDL_FreeSurface(_surface);
}

unsigned int SDLSurfaceResource::usedMem() const
{
    return _surface->pitch * _surface->h;
}

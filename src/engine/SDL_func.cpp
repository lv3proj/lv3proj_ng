#include <SDL/SDL.h>
#include "SDL_func.h"
#include "common.h"

Uint32 SDLfunc_GetSurfaceBytes(SDL_Surface *surface)
{
    return surface->pitch * surface->h;
}

/*
* Return the pixel value at (x, y)
* NOTE: The surface must be locked before calling this!
*/
Uint32 SDLfunc_getpixel(SDL_Surface *surface, int x, int y)
{
    ASSERT(surface->format->BytesPerPixel == 4);

    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32*)p;
}

/*
* Set the pixel at (x, y) to the given value
* NOTE: The surface must be locked before calling this!
*/
void SDLfunc_putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    ASSERT(surface->format->BytesPerPixel == 4);

    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)p = pixel;
}

SDL_Surface *CreateEmptySurfaceFrom(SDL_Surface *src)
{
    if(!src)
        return NULL;

    ASSERT(src->format->BytesPerPixel == 4);

    SDL_Surface *dest = SDL_CreateRGBSurface(src->flags, src->w, src->h, src->format->BitsPerPixel,
        src->format->Rmask,  src->format->Gmask,  src->format->Bmask,  src->format->Amask);
    return dest;
}

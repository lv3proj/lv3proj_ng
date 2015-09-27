#ifndef SDL_FUNC_H
#define SDL_FUNC_H

#include <SDL/SDL.h>

Uint32 SDLfunc_GetSurfaceBytes(const SDL_Surface *surface);

Uint32 SDLfunc_getpixel(SDL_Surface *surface, int x, int y);
void SDLfunc_putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
inline void SDLfunc_putpixel_safe(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    if(x > 0 && x < surface->w && y > 0 && y < surface->h)
        SDLfunc_putpixel(surface, x, y, pixel);
}

SDL_Surface *CreateEmptySurfaceFrom(SDL_Surface *src);

/*
void SDLfunc_drawRectangle(SDL_Surface *target, SDL_Rect& rectangle, int r, int g, int b, int a);
void SDLfunc_drawRectangle(SDL_Surface *target, SDL_Rect& rectangle, Uint32 pixel);
void SDLfunc_drawVLine(SDL_Surface *target, int x, int y1, int y2, int r, int g, int b, int a);
void SDLfunc_drawVLine(SDL_Surface *target, int x, int y1, int y2, Uint32 pixel);
void SDLfunc_drawHLine(SDL_Surface *target, int x1, int y, int x2, int r, int g, int b, int a);
void SDLfunc_drawHLine(SDL_Surface *target, int x1, int y, int x2, Uint32 pixel);
void SDLfunc_drawLine(SDL_Surface *target, int x1, int y1, int x2, int y2, int r, int g, int b, int a);
void SDLfunc_drawLine (SDL_Surface* target, int x1, int y1, int x2, int y2, Uint32 c);
*/


#endif

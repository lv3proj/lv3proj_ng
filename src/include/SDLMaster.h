#ifndef X_SDL_MASTER_H
#define X_SDL_MASTER_H

#if defined(LV3_USE_SYSTEM_SDL)
#error fixme
#include <SDL.h>
#include <SDL_syswm.h>
#elif defined(LV3_FORCE_OLD_SDL)
#error fixme
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#else
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#endif

#define IS_SDL2 SDL_VERSION_ATLEAST(2, 0, 0)

#endif

#include "SDLMusicResource.h"
#include <SDL/SDL_Mixer.h>
#include "log.h"

SDLMusicResource::SDLMusicResource(const char *name, Mix_Music *m, SDL_RWops *rwop)
 : Resource(name, RESOURCE_MUSIC)
 , _music(m), _rwop(rwop), _looppoint(0)
{
}

SDLMusicResource::~SDLMusicResource()
{
    Mix_MusicType musType = Mix_GetMusicType(_music);
    DBG logdebug("ResourceMgr:: Deleting Mix_Music "PTRFMT" (music type %u)", _music, musType);
    Mix_FreeMusic(_music);
    // FIXME!!
    // For unknown reason, OGG frees the RWop, but MikMod and WAV do not (not sure about the rest).
    // We have to check for that to prevent double-free,
    // Setting it to NULL here will prevent a second deletion & crash further down.
    if(musType == MUS_OGG)
        _rwop = NULL;

    if(_rwop)
        SDL_FreeRW(_rwop);
}


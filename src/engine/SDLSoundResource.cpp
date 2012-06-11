#include "SDLSoundResource.h"
#include <SDL/SDL_mixer.h>

SDLSoundResource::SDLSoundResource(const char *name, Mix_Chunk *ch)
 : Resource(name, RESOURCE_SOUND)
 , _chunk(ch), _looppoint(0)
{
}

SDLSoundResource::~SDLSoundResource()
{
    Mix_FreeChunk(_chunk);
}

unsigned int SDLSoundResource::usedMem() const
{
    return _chunk->alen;
}

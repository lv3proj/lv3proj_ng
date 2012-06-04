#include "SDLSoundResource.h"
#include <SDL/SDL_mixer.h>

SDLSoundResource::SDLSoundResource(const char *name, Mix_Chunk *ch)
 : Resource(name, RESOURCE_SOUND)
 , _chunk(ch)
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

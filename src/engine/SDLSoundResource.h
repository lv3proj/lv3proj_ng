#ifndef SDL_SOUND_RESOURCE_H
#define SDL_SOUND_RESOURCE_H

#include "Resource.h"

struct Mix_Chunk;


class SDLSoundResource : public Resource
{
public:
    SDLSoundResource(const char *name, Mix_Chunk *ch);

    inline Mix_Chunk *getChunk() const { return _chunk; }
    inline float getLoopPoint() const { return _looppoint; }

    virtual unsigned int usedMem() const;

protected:
    ~SDLSoundResource();

    Mix_Chunk *_chunk;
    float _looppoint;
};

#endif

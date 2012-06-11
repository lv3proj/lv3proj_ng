#ifndef SDL_MUSIC_RESOURCE_H
#define SDL_MUSIC_RESOURCE_H

#include "Resource.h"

typedef struct _Mix_Music Mix_Music;
struct SDL_RWops;


class SDLMusicResource : public Resource
{
public:
    SDLMusicResource(const char *name, Mix_Music *s, SDL_RWops *rwop);

    inline Mix_Music *getMusic() const { return _music; }
    inline float getLoopPoint() const { return _looppoint; }

protected:
    virtual ~SDLMusicResource();

    Mix_Music *_music;
    SDL_RWops *_rwop;
    float _looppoint;
};

#endif

#ifndef SOUNDCORE_H
#define SOUNDCORE_H

#include "ResourceMgr.h"
#include "SDLSoundResource.h"
#include "ScriptObject.h"

#include <vector>
#include <map>
#include <list>

typedef struct _Mix_Music Mix_Music;

struct gme_t;

class SoundFile : public ScriptObject
{
    friend class SoundCore;

public:

    void Play(int loops = 0); // also used to resume
    void Stop();
    void SetVolume(float vol);
    float GetVolume();
    bool IsPlaying();
    void FadeOut(float secs);
    void FadeIn(float secs, int loops = 0);
    //void Pause(); // NYI
    //void Seek(uint32); // NYI
    bool CanBeDeleted();
    void SetDeleteWhenStopped(bool del);

private:
    SoundFile(SDLSoundResource *);
    virtual ~SoundFile();

    CountedPtr<SDLSoundResource> _res;
    int _channel;
    bool _deletable;
};

typedef std::list<SoundFile*> SoundList;
typedef std::map<std::string, SoundList> SoundStore;

class SoundCore
{
public:
    SoundCore();
    ~SoundCore();

    bool Init();
    void Shutdown();
    void ClearGarbage();
    SoundFile *GetSound(const char *fn); // do NOT forget to decRef the returned ptr !!
    bool PlayMusic(const char *fn);
    void PauseMusic();
    void FadeOutMusic(float t);
    // TODO: proper fade-in or crossfading is quite impossible with SDL_Mixer.
    // This needs to wait until something OpenAL-based is in use instead.
    void StopMusic();
    bool IsPlayingMusic();
    void SetMusicVolume(float vol);
    float GetMusicVolume();

    inline SDLMusicResource *_GetMusicPtr() { return _music.content(); }
    inline void SetLoopPoint(float msec) { _looppoint = msec; }
    inline float GetLoopPoint() { return _looppoint; }
    inline uint32 GetSampleSize() { return _sampleSize; }

    void _ChannelFinished(int channel);
    SoundFile *_GetActiveChannel(int channel) { return _activeChannels[channel]; }
    void _SetActiveChannel(int channel, SoundFile *ch) { _activeChannels[channel] = ch; }

private:
    bool _LoadWithGME(MemResource *memRes);
    CountedPtr<SDLMusicResource> _music;
    gme_t *_gme;
    float _looppoint;
    float _volume;

    // queried from SDL
    int _sampleRate;
    int _channels; // output channels. 1 is mono, 2 is stereo.
    int _playChannels; // amount of playback channels - this many sounds can be played at a time.
    uint32 _sampleSize;

    SoundStore _sounds;

    std::vector<SoundFile*> _activeChannels;

};



#endif

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// Game_Music_Emu
#include "gme/gme.h"
#include "gme/Music_Emu.h"

#include "common.h"
#include "ResourceMgr.h"
#include "SoundCore.h"
#include "SDLMusicResource.h"
#include "SDLSoundResource.h"
#include "MemResource.h"

static SoundCore *_sndcore = NULL;

static void musicFinished(void)
{
    // from the manual:
    // NOTE: NEVER call SDL_Mixer functions, nor SDL_LockAudio, from a callback function. 
    // oh well... works. bah.
    logdev("SoundCore: Music finished, looppoint = %f", _sndcore->GetLoopPoint());
    if(_sndcore->_GetMusicPtr() && _sndcore->GetLoopPoint() >= 0)
    {
        Mix_PlayMusic(_sndcore->_GetMusicPtr()->getMusic(), 0);
        Mix_SetMusicPosition(_sndcore->GetLoopPoint());
    }
}

static void play_music_gme(void *udata, Uint8 *stream, int len)
{
    gme_t *gme = (gme_t*)udata;
    gme_play(gme, len / sizeof(Music_Emu::sample_t), (Music_Emu::sample_t*)stream);
}

static void stop_music_gme(void *userdata)
{
    logdev("SoundCore: delete GME, mem ptr "PTRFMT, userdata);
    MemResource *memRes = (MemResource*)userdata;
    memRes->decref();
}

static void channelFinished(int channel)
{
    //logdev("Channel finished: %d", channel);
    _sndcore->_ChannelFinished(channel);
}


SoundFile::SoundFile(SDLSoundResource *res)
: _res(res), _channel(-1), _deletable(false)
{
    res->incref();
}

SoundFile::~SoundFile()
{
    Stop();
    _res->decref();
}

void SoundFile::SetVolume(float vol)
{
    Mix_VolumeChunk(_res->getChunk(), int(vol * MIX_MAX_VOLUME));
}

float SoundFile::GetVolume()
{
    return Mix_VolumeChunk(_res->getChunk(), -1) / float(MIX_MAX_VOLUME);
}

void SoundFile::Play(int loops /* = 0 */)
{
    _channel = Mix_PlayChannel(-1, _res->getChunk(), loops);
    if(_channel < 0)
    {
        logdebug("No free channels!");
        return;
    }

    //logdebug("PLAY on channel %u", channel);

    _sndcore->_SetActiveChannel(_channel, this);

}

bool SoundFile::IsPlaying()
{
    if(_channel < 0)
        return false;
    SoundFile *chs = _sndcore->_GetActiveChannel(_channel); // Not using SDL_Mixer function, because it crashes...
    if(!chs)
        return false;
    if(chs != this)
        return false;

    return Mix_Playing(_channel);
}

void SoundFile::Stop()
{
    if(IsPlaying())
    {
        Mix_HaltChannel(_channel);
        _channel = -1;
    }
}

bool SoundFile::CanBeDeleted()
{
    return _deletable && !IsPlaying();
}

void SoundFile::SetDeleteWhenStopped(bool del)
{
    _deletable = del;
}

void SoundFile::FadeOut(float secs)
{
    if(_channel > 0)
        Mix_FadeOutChannel(_channel, int(secs * 1000));
}

void SoundFile::FadeIn(float secs, int loops /* = 0 */)
{
    Mix_FadeInChannel(-1, _res->getChunk(), loops, int(secs * 1000));
}



SoundCore::SoundCore()
{
    _sndcore = this;
}

SoundCore::~SoundCore()
{
    _sndcore = NULL;
}

bool SoundCore::Init()
{
    _music = NULL;
    _gme = NULL;
    _volume = 1;
    _looppoint = 0;

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); // 2 means stereo

    _playChannels = Mix_AllocateChannels(64);
    _activeChannels.resize(_playChannels, (SoundFile*)NULL);

    Uint16 format;
    Mix_QuerySpec(&_sampleRate, &format, &_channels);

    const char *fmt = "Unknown";
    _sampleSize = 1;
    switch(format)
    {
        case AUDIO_U8:     fmt = "U8"; break;
        case AUDIO_S8:     fmt = "S8"; break;
        case AUDIO_U16LSB: fmt = "U16LSB"; _sampleSize = 2; break;
        case AUDIO_S16LSB: fmt = "S16LSB"; _sampleSize = 2; break;
        case AUDIO_U16MSB: fmt = "U16MSB"; _sampleSize = 2; break;
        case AUDIO_S16MSB: fmt = "S16MSB"; _sampleSize = 2; break;
    }
    if(!(_sampleRate == 44100 && _sampleSize == 2 && _channels == 2))
    {
        logerror("SoundCore: %d Hz, %d output channels [%s], %u playback channels", _sampleRate, _channels, fmt, _playChannels);
        logerror("--> WARNING: Unexpected Mixer settings, audio playback may be weird!");
    }
    else
        logdetail("SoundCore: %d Hz, %d output channels [%s], %u playback channels", _sampleRate, _channels, fmt, _playChannels);

    Mix_ChannelFinished(channelFinished);

    return true;
}

void SoundCore::Shutdown()
{
    StopMusic();

    for(SoundStore::iterator s = _sounds.begin(); s != _sounds.end(); ++s)
    {
        SoundList& sl = s->second;
        for(SoundList::iterator it = sl.begin(); it != sl.end(); ++it)
            delete *it;
    }

    _sounds.clear();

    Mix_CloseAudio();
}

bool SoundCore::PlayMusic(const char *fn)
{
    if(!fn)
    {
        if(Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            return true;
        }
        return false;
    }

    SDLMusicResource *mus = resMgr.LoadMusic(fn);

    if(mus)
    {
        if(_music == mus)
        {
            mus->decref();
            return true;
        }

        SetLoopPoint(mus->getLoopPoint());

        StopMusic();

        int result = Mix_PlayMusic(mus->getMusic(), 0);
        if(!result) // everything ok
        {
            if(_music)
                _music->decref();
            _music = mus;
            Mix_HookMusicFinished(musicFinished);
            return true;
        }
        else
            mus->decref();
    }

    // SDL_Mixer can't play it...

    // get raw memory block - should still be in memory
    std::string file("music/");
    file += fn;
    MemResource *memRes = resMgr.LoadFile(file.c_str());
    if(memRes)
    {
        // Try GME
        if(_LoadWithGME(memRes)
            // ... possibly more loaders here later ...
            )
        {
            return true; // all fine now
        }

        // unable to load, memory no longer used here
        memRes->decref();
    }

    // still no success, give up
    logerror("LoadMusic failed: '%s'", file.c_str());
    return false;
}

void SoundCore::PauseMusic()
{
    Mix_PauseMusic();
}

void SoundCore::FadeOutMusic(float t)
{
    Mix_FadeOutMusic(int(t * 1000));
    _looppoint = -1; // Do not restart after fading out
}

void SoundCore::StopMusic()
{
    Mix_HookMusic(NULL, NULL); // clear any custom player function
    Mix_HaltMusic();

    // be sure audio thread is not active
    SDL_LockAudio();
    SDL_UnlockAudio();

    if(_music)
    {
        _music->decref();
        _music = NULL;
    }

    if(_gme)
    {
        gme_delete(_gme);
        _gme = NULL;
    }
}

bool SoundCore::IsPlayingMusic()
{
    return Mix_PlayingMusic() && !Mix_PausedMusic();
}

void SoundCore::SetMusicVolume(float vol)
{
    int ivol = int(vol * MIX_MAX_VOLUME);
    if(ivol > MIX_MAX_VOLUME)
        ivol = MIX_MAX_VOLUME;
    Mix_VolumeMusic(ivol);
    if(_gme)
        gme_set_volume(_gme, vol);
    _volume = vol;
}

float SoundCore::GetMusicVolume()
{
    return _volume;
}

SoundFile *SoundCore::GetSound(const char *fn)
{
    // First, try to find a sound that isn't in use anymore that can be recycled.
    SoundList &used = _sounds[fn];
    SoundFile *soundf = NULL;
    for(SoundList::iterator it = used.begin(); it != used.end(); ++it)
    {
        soundf = *it;
        if(soundf->CanBeDeleted())
        {
            logdev("SoundCore: Recycling "PTRFMT" (%s)", fn);
            soundf->SetDeleteWhenStopped(false); // Revive it
            return soundf;
        }
    }

    soundf = NULL;
    SDLSoundResource *res = resMgr.LoadSound(fn);
    if(res)
    {
        soundf = new SoundFile(res);
        res->decref();
        used.push_back(soundf);
    }
    return soundf;
}

void SoundCore::ClearGarbage()
{
    for(SoundStore::iterator s = _sounds.begin(); s != _sounds.end(); ++s)
    {
        SoundList& sl = s->second;
        for(SoundList::iterator it = sl.begin(); it != sl.end(); )
        {
            SoundFile *sf = *it;
            if(sf->CanBeDeleted())
            {
                delete sf;
                sl.erase(it++);
            }
            else
                ++it;
        }
    }
}

bool SoundCore::_LoadWithGME(MemResource *memRes)
{
    if(!(_sampleSize == 2 && _channels == 2))
    {
        logerror("SoundCore: Game_Music_Emu needs 16 bit samples and stereo output");
        return false;
    }

    gme_t *emu =  NULL;
    gme_err_t err = gme_open_data(memRes->ptr(), (long)memRes->size(), &emu, _sampleRate);
    
    if(!err)
    {
        const char *ty = gme_type_system(gme_type(emu));
        logdebug("_LoadWithGME() ptr = "PTRFMT"  Type: %s" , memRes->ptr(), ty);
        StopMusic();
        _gme = emu;
        gme_start_track(emu, 0); // TODO FIXME: add support for other track IDs (NSF files)
        gme_set_user_cleanup(emu, stop_music_gme);
        gme_set_user_data(emu, memRes);
        Mix_HookMusic(play_music_gme, (void*)emu);
        gme_set_volume(emu, _volume);
        return true;
    }

    return false;
}

void SoundCore::_ChannelFinished(int channel)
{
    _SetActiveChannel(channel, NULL);
}

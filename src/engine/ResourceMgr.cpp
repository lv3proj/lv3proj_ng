#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <assert.h>
#include "SDL_func.h"

#include "common.h"
#include "FileAPI.h"
#include "ResourceMgr.h"
#include "mathtools.h"
#include "Engine.h"

#include "SDLSurfaceResource.h"
#include "SDLMusicResource.h"
#include "SDLSoundResource.h"
#include "MemResource.h"
#include "Texture.h"
#include "Tile.h"


static bool exists(const std::string& fn)
{
    return !!vfs.GetFile(fn.c_str());
}

static bool exists(const char *fn)
{
    return !!vfs.GetFile(fn);
}


ResourceMgr::ResourceMgr()
: _usedMem(0)
{
}

ResourceMgr::~ResourceMgr()
{
}

void ResourceMgr::Shutdown()
{
    DropUnused();
    for(int i = 0; i < RESOURCE_MAX; ++i)
    {
        ResourceStore& res = _res[i];

        for(ResourceStore::iterator it = res.begin(); it != res.end(); ++it)
        {
            Resource *r = it->second.content();
            logerror("WARNING: [type %u] Forgot to free resource %s (ref %u, mem %u)",
                r->type(), r->name(), r->refcount(), r->usedMem());
        }
    }
}

void ResourceMgr::DropUnused(void)
{
    logdev("ResourceMgr before clear: %u resources, %u KB", GetUsedCount(), GetUsedMem() / 1024);
    // Start with resources that are likely to have the most dependencies
    bool changed;
    do
    {
        changed = false;
        for(int i = RESOURCE_MAX-1; i >= 0; --i)
        {
            ResourceStore& res = _res[i];

            for(ResourceStore::iterator it = res.begin(); it != res.end(); )
            {
                assert(it->second->refcount());
                if(it->second->refcount() <= 1) // we are the only one left holding a reference?
                {
                    changed = true;
                    logdev("Delete resource [%s], %u KB", it->second->name(), it->second->usedMem() / 1024);
                    _unaccountMem(it->second->usedMem());
                    res.erase(it++); // decrefs, and deletes
                }
                else
                {
                    logdev("* Keep resource [%s], %u KB", it->second->name(), it->second->usedMem() / 1024);
                    ++it;
                }
            }
        }
    }
    while(changed);
    vfs.ClearGarbage();

    logdev("ResourceMgr after clear: %u resources, %u KB", GetUsedCount(), GetUsedMem() / 1024);
}

void ResourceMgr::Update(float dt)
{
    for(int i = 0; i < RESOURCE_MAX; ++i)
    {
        ResourceStore& res = _res[i];
        for(ResourceStore::iterator it = res.begin(); it != res.end(); ++it)
            it->second->update(dt);
    }
}

unsigned int ResourceMgr::GetUsedCount(void)
{
    unsigned int total = 0;
    for(int i = 0; i < RESOURCE_MAX; ++i)
    {
        total += (unsigned int)_res[i].size();
    }
    return total;
}

unsigned int ResourceMgr::GetUsedMem(void)
{
    return _usedMem;
}

void ResourceMgr::_accountMem(unsigned int bytes)
{
    _usedMem += bytes;
}

void ResourceMgr::_unaccountMem(unsigned int bytes)
{
    _usedMem -= bytes;
}

Resource *ResourceMgr::_GetResource(ResourceType type, const char *name)
{
    ResourceStore::iterator it = _res[type].find(name);
    return it != _res[type].end() ? it->second.content() : NULL;
}

void ResourceMgr::Add(CountedPtr<Resource> r)
{
    if(_res[r->type()].insert(std::make_pair(r->name(), r)).second)
        _accountMem(r->usedMem());
    else
        logdebug("WARNING: Resource '%s' not added, already exists", r->name());
}

SDLSurfaceResource *ResourceMgr::LoadImg(const char *name)
{
    std::string realname = "gfx/";
    realname += name;

    //TODO: add extensions automatically?

    return _LoadImgInternal(realname.c_str());
}

SDLSurfaceResource *ResourceMgr::_LoadImgInternal(const char *name)
{
    SDLSurfaceResource *imgRes = (SDLSurfaceResource*)_GetResource(RESOURCE_IMAGE, name);
    if(imgRes)
        return imgRes;

    SDL_Surface *img = NULL;
    ttvfs::VFSFile *vf = NULL;
    SDL_RWops *rwop = NULL;

    vf = vfs.GetFile(name);
    if(vf && vf->size())
    {
        rwop = SDL_RWFromMem((void*)vf->getBuf(), vf->size());
        if(rwop)
        {
            img = IMG_Load_RW(rwop, 0);
            SDL_RWclose(rwop);
        }
        vf->dropBuf(true); // delete original buf -- even if it could not load an image from it - its useless to keep this memory
    }

    if(!img)
    {
        logerror("LoadImg failed: '%s'", name);
        return NULL;
    }

    if(img->format->BitsPerPixel != 32)
    {
        SDL_PixelFormat fmt = *img->format;
        fmt.BitsPerPixel = 32;
        fmt.BytesPerPixel = 4;
        fmt.alpha = 0xff;
        fmt.Rmask = 0x000000ff; // TODO: fix for big endian
        fmt.Gmask = 0x0000ff00;
        fmt.Bmask = 0x00ff0000;
        fmt.Amask = 0xff000000;
        SDL_Surface *newimg = SDL_ConvertSurface(img, &fmt, 0);
        if(newimg && img != newimg)
        {
            SDL_FreeSurface(img);
            img = newimg;
        }
    }

    logdebug("LoadImg: '%s' [%s] -> "PTRFMT , name, vf ? vf->getType() : "*", img);

    imgRes = new SDLSurfaceResource(name, img);
    Add(imgRes);

    return imgRes;
}

SDLMusicResource *ResourceMgr::LoadMusic(const char *name)
{
    std::string fn("music/");
    fn += name;

    SDLMusicResource *musicRes = (SDLMusicResource*)_GetResource(RESOURCE_MUSIC, name);
    if(musicRes)
        return musicRes;

    MemResource *mr = _LoadFileInternal(fn.c_str(), false);
    SDL_RWops *rwop = NULL;
    Mix_Music *music = NULL;
    if(!mr || !mr->size())
    {
        logerror("LoadMusic failed: '%s' (file not found or empty)", name);
        return NULL;
    }
    if(mr && mr->size())
    {
        rwop = SDL_RWFromConstMem((const void*)mr->ptr(), mr->size());
        music = Mix_LoadMUS_RW(rwop);
        // We can NOT free the RWop here, it is still used by SDL_Mixer to access the music data.
        // Instead, is is saved with the other pointers and freed along with the music later.

        if(!music)
        {
            // SDL_mixer didn't recognize the format...
            SDL_RWclose(rwop);
            return NULL;
        }
    }
    
    logdebug("LoadMusic: '%s' -> "PTRFMT , name, music);

    musicRes = new SDLMusicResource(name, music, rwop);
    musicRes->addDep(mr);

    Add(musicRes);

    return musicRes;
}

SDLSoundResource *ResourceMgr::LoadSound(const char *name)
{
    std::string fn("sfx/");
    fn += name;

    SDLSoundResource *soundRes = (SDLSoundResource*)_GetResource(RESOURCE_SOUND, fn.c_str());
    if(soundRes)
        return soundRes;

    ttvfs::VFSFile *vf = vfs.GetFile(fn.c_str());
    if(!vf)
    {
        logerror("LoadSound failed: '%s' (file not found)", fn.c_str());
        return NULL;
    }

    SDL_RWops *rwop = SDL_RWFromMem((void*)vf->getBuf(), vf->size());
    Mix_Chunk *sound = Mix_LoadWAV_RW(rwop, 0);
    SDL_RWclose(rwop);
    vf->dropBuf(true); // after decoding, original memory is no longer needed

    if(!sound)
    {
        logerror("LoadSound failed: '%s' (format not recognized)", fn.c_str());
        return NULL;
    }

    logdebug("LoadSound: '%s' [%s] -> "PTRFMT , name, vf->getType(), sound);

    soundRes = new SDLSoundResource(fn.c_str(), sound);
    Add(soundRes);

    return soundRes;
}

MemResource *ResourceMgr::LoadFile(const char *name)
{
    return _LoadFileInternal(name, false);
}

MemResource *ResourceMgr::LoadTextFile(const char *name)
{
    return _LoadFileInternal(name , true);
}

MemResource *ResourceMgr::_LoadFileInternal(const char *name, bool textmode)
{

    MemResource *memRes = (MemResource*)_GetResource(RESOURCE_MEM, name);
    if(memRes)
        return memRes;

    ttvfs::VFSFile *vf = vfs.GetFile(name);
    
    if(!vf)
    {
        logerror("LoadFile failed: '%s'", name);
        return NULL;
    }

    const char *buf = (const char*)vf->getBuf();
    vf->dropBuf(false);
    size_t size = vf->size();

    logdebug("LoadFile: '%s' [%s], %u bytes at ["PTRFMT"]" , name, vf->getType(), (uint32)size, buf);

    memRes = new MemResource(name, buf, size);
    Add(memRes);

    return memRes;
}

Texture *ResourceMgr::_GetTexture(const char *name)
{
    return (Texture*)_GetResource(RESOURCE_TEXTURE, name);
}



// extern, global
ResourceMgr resMgr;


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <assert.h>
#include "SDL_func.h"

#include "common.h"
#include "FileAPI.h"
#include "ResourceMgr.h"
#include "mathtools.h"
//#include "AnimParser.h"
//#include "PropParser.h"

#include "SDLSurfaceResource.h"
#include "SDLMusicResource.h"
#include "SDLSoundResource.h"
#include "MemResource.h"
#include "Texture.h"



// splits string like "filename.anim:4:default" or "image.png:32:32:16:16". leaves unused args unchanged!
static void SplitFilenameToProps(const char *in, std::string *fn, std::string *s1 /* = NULL */,
                          std::string *s2 /* = NULL */, std::string *s3 /* = NULL */, std::string *s4 /* = NULL */,
                          std::string *s5 /* = NULL */)
{
    std::vector<std::string> fields;
    StrSplit(in, ":", fields, true);
    if(fields.size() >= 1 && fn)
        *fn = fields[0];
    if(fields.size() >= 2 && s1)
        *s1 = fields[1];
    if(fields.size() >= 3 && s2)
        *s2 = fields[2];
    if(fields.size() >= 4 && s3)
        *s3 = fields[3];
    if(fields.size() >= 5 && s4)
        *s4 = fields[4];
    if(fields.size() >= 6 && s5)
        *s5 = fields[5];
}


ResourceMgr::ResourceMgr()
: _usedMem(0)
{
}

ResourceMgr::~ResourceMgr()
{
}


void ResourceMgr::DropUnused(void)
{
    // Start with resources that are likely to have the most dependencies
    for(int i = RESOURCE_MAX-1; i >= 0; --i)
    {
        ResourceStore& res = _res[i];

        for(ResourceStore::iterator it = res.begin(); it != res.end(); )
        {
            if(!it->second->refcount())
            {
                delete it->second;
                res.erase(it++);
            }
            else
                ++it;
        }
    }
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
        total += (uint32)_res[i].size();
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
    return it != _res[type].end() ? it->second : NULL;
}

void ResourceMgr::Add(Resource *r)
{
    if(_res[r->type()].insert(std::make_pair<std::string, Resource*>(r->name(), r)).second)
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
    std::string fn,s1,s2,s3,s4,s5;
    SplitFilenameToProps(name, &fn, &s1, &s2, &s3, &s4, &s5);

    SDLSurfaceResource *imgRes = (SDLSurfaceResource*)_GetResource(RESOURCE_IMAGE, name);
    if(imgRes)
    {
        imgRes->incref();
        return imgRes;
    }

    SDL_Surface *img = NULL;
    ttvfs::VFSFile *vf = NULL;
    SDL_RWops *rwop = NULL;

    // we got additional properties
    if(fn != name)
    {
        SDLSurfaceResource *originRes = _LoadImgInternal(fn.c_str());
        SDL_Surface *origin = originRes ? originRes->getSurface() : NULL;
        if(origin)
        {
            SDL_Rect rect;
            rect.x = atoi(s1.c_str());
            rect.y = atoi(s2.c_str());
            rect.w = atoi(s3.c_str());
            rect.h = atoi(s4.c_str());
            if(!rect.w)
                rect.w = origin->w - rect.x;
            if(!rect.h)
                rect.h = origin->h - rect.y;
            bool flipH = s5.find('h') != std::string::npos;
            bool flipV = s5.find('v') != std::string::npos;

            SDL_Surface *section = SDL_CreateRGBSurface(origin->flags, rect.w, rect.h, origin->format->BitsPerPixel,
                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000); // TODO: fix this for big endian
            
            // properly blit alpha values, save original flags + alpha before blitting, and restore after
            uint8 oalpha = origin->format->alpha;
            uint8 oflags = origin->flags;
            SDL_SetAlpha(origin, 0, 0); 
            SDL_BlitSurface(origin, &rect, section, NULL);
            origin->format->alpha = oalpha;
            origin->flags = oflags;

            if(flipH && flipV)
            {
                img = SurfaceFlipHV(section);
                //SDL_FreeSurface(section);
            }
            else if(flipH)
            {
                img = SurfaceFlipH(section);
                //SDL_FreeSurface(section);
            }
            else if(flipV)
            {
                img = SurfaceFlipV(section);
                //SDL_FreeSurface(section);
            }
            else
            {
                img = section;
            }
            originRes->decref();
        }
    }
    else // nothing special, just load image normally
    {
        vf = vfs.GetFile(fn.c_str());
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
    }
    if(!img)
    {
        logerror("LoadImg failed: '%s'", name);
        return NULL;
    }

    // no longer doing software blit so this should no longer be required
    /*SDL_Surface *newimg = SDL_DisplayFormatAlpha(img);
    if(newimg && img != newimg)
    {
        SDL_FreeSurface(img);
        img = newimg;
    }*/

    logdebug("LoadImg: '%s' [%s] -> "PTRFMT , name, vf ? vf->getType() : "*", img);

    imgRes = new SDLSurfaceResource(name, img);
    Add(imgRes);

    return imgRes;
}

/*
Anim *ResourceMgr::LoadAnim(const char *name)
{
    std::string fn("gfx/");
    std::string relpath(_PathStripLast(name));
    std::string loadpath;
    fn += name;

    Anim *ani = (Anim*)_GetPtr(fn);
    if(ani)
    {
        _IncRef((void*)ani);
    }
    else
    {
        // a .anim file is just a text file, so we use the internal text file loader
        memblock *mb = _LoadTextFileInternal((char*)fn.c_str(), FILENAME_TMP_INDIC);
        if(!mb)
        {
            logerror("LoadAnim: Failed to open '%s'", fn.c_str());
            return NULL;
        }

        ani = ParseAnimData((char*)mb->ptr, (char*)fn.c_str());
        Drop(mb); // text data are no longer needed

        if(!ani)
        {
            logerror("LoadAnim: Failed to parse '%s'", fn.c_str());
            return NULL;
        }

        // load all additional files referenced in this .anim file
        // pay attention to relative paths in the file, respect the .anim file's directory for this
        for(AnimMap::iterator am = ani->anims.begin(); am != ani->anims.end(); am++)
            for(AnimFrameVector::iterator af = am->second.store.begin(); af != am->second.store.end(); af++)
            {
                loadpath = AddPathIfNecessary(af->filename,relpath);
                af->surface = LoadImg(loadpath.c_str()); // get all images referenced
                if(af->surface)
                    af->callback.ptr(af->surface); // register callback for auto-deletion
                else
                {
                    logerror("LoadAnim: '%s': Failed to open referenced image '%s'", fn.c_str(), loadpath.c_str());
                    // we keep the NULL-ptr anyways
                }
            }

         logdebug("LoadAnim: '%s' [%s] -> "PTRFMT , fn.c_str(), vfs.GetFile(fn.c_str())->getType(), ani); // the file must exist

         _SetPtr(fn, (void*)ani);
         _InitRef((void*)ani, RESTYPE_ANIM);
    }

    return ani;
}
*/

SDLMusicResource *ResourceMgr::LoadMusic(const char *name)
{
    std::string fn("music/");
    fn += name;

    // HACK: In this method, we use FILENAME_MUSIC_INDIC to give the music file a different name then its raw data.
    // This is necessary because SoundCore may read a file as raw binary in case SDL can't handle it internally,
    // but we still need to differentiate pointers by type. This fixes some crash problems on music unload.
    SDLMusicResource *musicRes = (SDLMusicResource*)_GetResource(RESOURCE_MUSIC, name);
    if(musicRes)
    {
        musicRes->incref();
        return musicRes;
    }

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
            logerror("LoadMusic failed: '%s' (format not recognized)", name);
            mr->decref();
            SDL_RWclose(rwop);
            return NULL;
        }
    }
    
    logdebug("LoadMusic: '%s' -> "PTRFMT , name, music);

    musicRes = new SDLMusicResource(name, music, rwop);
    musicRes->addDep(mr);
    mr->decref(); // we don't actually mean to keep it referenced, as addDep() already increased the count

    Add(musicRes);

    return musicRes;
}

SDLSoundResource *ResourceMgr::LoadSound(const char *name)
{
    std::string fn("sfx/");
    fn += name;

    SDLSoundResource *soundRes = (SDLSoundResource*)_GetResource(RESOURCE_SOUND, fn.c_str());
    if(soundRes)
    {
        soundRes->incref();
        return soundRes;
    }

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
    {
        memRes->incref();
        return memRes;
    }

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
    Texture *tex = (Texture*)_GetResource(RESOURCE_TEXTURE, name);
    if(tex)
        tex->incref();
    return tex;
}



// extern, global (since we aren't using singletons here)
ResourceMgr resMgr;


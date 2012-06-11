#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <map>

#include "Resource.h"

class SDLSurfaceResource;
class SDLMusicResource;
class SDLSoundResource;
class MemResource;
class Texture;
class Anim;

typedef std::map<std::string, Resource*> ResourceStore;

class ResourceMgr
{

public:
    ResourceMgr();
    ~ResourceMgr();

    void DropUnused(void);
    void Add(Resource *r);
    void Update(float dt);


    SDLSurfaceResource *LoadImg(const char *name);

    Anim *LoadAnim(const char *name);
    SDLMusicResource *LoadMusic(const char *name);
    SDLSoundResource *LoadSound(const char *name);
    MemResource *LoadFile(const char *name);
    MemResource *LoadTextFile(const char *name);

    Texture *_GetTexture(const char *name); // does *NOT* load a texture if not known. The engine does texture management.

    unsigned int GetUsedCount(void); // amount of resources
    unsigned int GetUsedMem(void); // estimated total resource memory consumption


    inline const ResourceStore& GetResourcesOfType(ResourceType type) const { return _res[type]; }


private:
    Resource *_GetResource(ResourceType type, const char *name);
    SDLSurfaceResource *_LoadImgInternal(const char *name);
    Anim *_LoadAnimInternal(const char *name);
    MemResource * _LoadFileInternal(const char *name, bool textmode);

    void _accountMem(unsigned int bytes);
    void _unaccountMem(unsigned int bytes);

    ResourceStore _res[RESOURCE_MAX];

    unsigned int _usedMem;
};


extern ResourceMgr resMgr;



#endif

#ifndef ANIM_H
#define ANIM_H

#include <map>
#include <vector>
#include <string>
#include "Texture.h"

enum AnimCmdEnum
{
    ANIMCMD_REPEAT, // set index back to 0 and repeat current sub-anim
    ANIMCMD_JUMP // jump to sub-anim with given name
};

struct AnimFrameCmd
{
    unsigned int action;
    std::string param;
};

class AnimFrame
{
    friend class ResourceMgr;

public:
    inline const char *GetTextureName(void) { return texname.c_str(); }
    inline Texture *GetTexture(void) { return tex; }
    unsigned int frametime;
    AnimFrame() : tex(NULL) {}
    AnimFrame(std::string& fn, unsigned int t) : tex(NULL), texname(fn), frametime(t) {}
    virtual ~AnimFrame()
    {
        if(tex)
            tex->decref();
    }
protected:

    std::string texname; // name of texture

    Texture *tex;
};

typedef std::vector<AnimFrame> AnimFrameVector;

struct AnimFrameStore
{
    std::string name;
    AnimFrameVector store;
    AnimFrameCmd cmd; // what do do after the animation is played
};

typedef std::map<std::string, AnimFrameStore> AnimMap;

class Anim : public Resource
{
public:
    Anim(const char *n)
        : Resource(n, RESOURCE_ANIM), first(NULL), mem(0)
    {
    }

    virtual ~Anim() {}

    AnimMap anims;
    AnimFrameStore *first;
    unsigned int mem;
};

#endif

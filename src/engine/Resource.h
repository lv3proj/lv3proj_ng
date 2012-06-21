#ifndef RESOURCE_BASE_H
#define RESOURCE_BASE_H

#include <string>
#include <list>


enum ResourceType
{
    RESOURCE_BASE,      // used for resources that other resources depend on, but that have no function otherwise.
    RESOURCE_MEM,
    RESOURCE_IMAGE, // possibly depends on another image
    RESOURCE_SOUND,
    RESOURCE_MUSIC, // TODO: merge this with sound whenever feasible
    RESOURCE_TEXTURE, // depends on an image
    RESOURCE_ANIM, // depends on multiple textures
    RESOURCE_TILE, // depends on Texture

    RESOURCE_MAX
};


class Resource
{
protected:
    Resource(const char *n, ResourceType t);

public:

    virtual ~Resource();
   
    inline const char *name() { return _name.c_str(); }
    inline ResourceType type() { return _type; }
    inline void incref() { ++_refcount; }
    inline unsigned int refcount() const { return _refcount; }
    void decref();

    void addDep(Resource *); // add other resource this resource depends on

    virtual void update(float dt) {}
    virtual unsigned int usedMem() const { return 0; }


protected:


private:

    unsigned int _refcount;
    std::string _name;
    ResourceType _type;
    std::list<Resource*> _dep;
};


#endif

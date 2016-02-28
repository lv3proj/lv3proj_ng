#ifndef RESOURCE_BASE_H
#define RESOURCE_BASE_H

#include <string>
#include <vector>
#include "refcounted.h"


enum ResourceType
{
    RESOURCE_BASE,      // used for resources that other resources depend on, but that have no function otherwise.
    RESOURCE_MEM,
    RESOURCE_IMAGE,
    RESOURCE_SOUND,
    RESOURCE_MUSIC, // TODO: merge this with sound whenever feasible
    RESOURCE_TEXTURE, // depends on an image

    RESOURCE_MAX
};


class Resource : public Refcounted
{
protected:
    Resource(const char *n, ResourceType t);

public:

    virtual ~Resource();
   
    inline const char *name() const { return _name.c_str(); }
    inline size_t nameLen() const { return _name.length(); }
    inline ResourceType type() const { return _type; }

    //void addDep(Resource *); // add other resource this resource depends on

    virtual void update(float dt) {}
    virtual unsigned int usedMem() const { return 0; }


protected:


private:
    ResourceType _type;
    std::string _name;
    //std::vector<CountedPtr<Resource> > _dep;
};


#endif

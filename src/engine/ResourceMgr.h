#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <vector>
#include "ImageResource.h"
#include "Texture.h"

namespace ttvfs { class Root; }

class ResourceMgr
{
public:
    static bool StaticInit();
    static void StaticShutdown();

    struct Frame
    {
        std::vector<CountedPtr<Resource> > res;
    };

    ResourceMgr();
    ~ResourceMgr();

    void pushFrame();
    void popFrame();
    unsigned getNumFrames() const;
    unsigned getEntriesForFrame(unsigned n) const;

    void add(Resource *);

    CountedPtr<ImageResource> getImage(const char *);
    CountedPtr<Texture> getTex(const char *); // used by engine
    //CountedPtr<Resource> getSound(const char *);

private:
    CountedPtr<Resource> *search(const char *, ResourceType ty);

    std::vector<Frame> _frames;
};


#endif

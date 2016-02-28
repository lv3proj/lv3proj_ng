#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include <vector>
#include "ImageResource.h"
#include "Texture.h"

namespace ttvfs { class Root; }
class Renderer;

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

    void setRenderer(Renderer *r);

    void pushFrame();
    void popFrame();

    void add(Resource *);

    CountedPtr<ImageResource> getImage(const char *);
    CountedPtr<Texture> getTex(const char *); // used by engine
    //CountedPtr<Resource> getSound(const char *);

private:
    CountedPtr<Resource> *search(const char *, ResourceType ty);

    std::vector<Frame> _frames;
    Renderer *render; // Needs to know the Renderer to transform Image into Texture
};


#endif

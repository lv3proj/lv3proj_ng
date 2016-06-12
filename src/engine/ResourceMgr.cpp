#include "common.h"
#include "ResourceMgr.h"
#include "ImageResource.h"
#include "Texture.h"
#include "io/image.h"
#include "renderer.h"

#define VFS_ENABLE_C_API 1
#include "vfs/ttvfs_cfileapi/ttvfs_stdio.h"
#include "vfs/ttvfs/ttvfs.h"

static void *loadBuf(const char *fn, size_t *psz)
{
    VFILE *vf = vfopen(fn, "rb");
    if(!vf)
        return NULL;
    size_t sz = 0;
    if(vfsize(vf, &sz) < 0)
        return NULL;

    char *buf = (char*)malloc(size_t(sz) + 1);
    size_t rd = vfread(buf, 1, sz, vf);
    vfclose(vf);
    if(rd != sz)
    {
        free(buf);
        return NULL;
    }
    buf[sz] = 0;
    if(psz)
        *psz = size_t(sz);
    return buf;
}

template<typename TO, typename FROM>
inline TO dyncast(FROM x)
{
#ifdef _DEBUG
    TO to = dynamic_cast<TO>(x);
    ASSERT(!!to);
    return to;
#else
    return static_cast<TO>(x);
#endif
}

static ttvfs::Root *vfsroot = NULL;

bool ResourceMgr::StaticInit()
{
    ASSERT(!vfsroot);
    if(!ttvfs::checkCompat())
        return false;
    vfsroot = ttvfs_newroot();
    ttvfs_setroot(vfsroot);
    vfsroot->AddLoader(new ttvfs::DiskLoader);
    return true;
}

void ResourceMgr::StaticShutdown()
{
    ASSERT(vfsroot);
    ttvfs_deleteroot(vfsroot);
    vfsroot = NULL;
}


ResourceMgr::ResourceMgr()
: render(NULL)
{
    ASSERT(vfsroot);
}

ResourceMgr::~ResourceMgr()
{
    while(_frames.size())
        popFrame();
}

void ResourceMgr::pushFrame()
{
    _frames.push_back(Frame());
}

void ResourceMgr::popFrame()
{
    _frames.pop_back();
}

void ResourceMgr::add(Resource *res)
{
    _frames.back().res.push_back(res);
}

CountedPtr<Resource> *ResourceMgr::search(const char *fn, ResourceType ty)
{
    const size_t sz = _frames.size();
    for(size_t i = 0; i < sz; ++i)
    {
        Frame& f = _frames[i];
        const size_t n = f.res.size();
        for(size_t r = 0; r < n; ++r)
        {
            Resource *p = f.res[r].content();
            if(p->type() == ty && !strcmp(p->name(), fn))
                return &f.res[r];
        }
    }
    return NULL;
}


CountedPtr<ImageResource> ResourceMgr::getImage(const char *fn)
{
    if(CountedPtr<Resource> *imr = search(fn, RESOURCE_IMAGE))
        return dyncast<ImageResource*>(imr->content());

    std::string path = "gfx/";
    path += fn;
    path += ".png";
    size_t sz;
    void *buf = loadBuf(path.c_str(), &sz);
    if(!buf)
        return NULL;
    Image *img = Image::LoadPNG(path.c_str(), sz);
    free(buf);
    if(!img)
    {
        logerror("Not a PNG file: %s", path.c_str());
        return NULL;
    }
    ImageResource *res = new ImageResource(path.c_str(), img);
    add(res);
    return res;
}

CountedPtr<Texture> ResourceMgr::getTex(const char *fn)
{
    if(CountedPtr<Resource> *texr = search(fn, RESOURCE_TEXTURE))
        return dyncast<Texture*>(texr->content());

    CountedPtr<ImageResource> img = getImage(fn);
    unsigned texid = render->loadTex(img->getImage());
    Texture *tex = new Texture(img.content(), texid);
    add(tex);
    return tex;
}

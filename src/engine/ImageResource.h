#ifndef SDL_SURFACE_RESOURCE_H
#define SDL_SURFACE_RESOURCE_H

#include "Resource.h"

class Image;

class ImageResource : public Resource
{
public:
    ImageResource(const char *name, Image *img);

    inline Image *getImage() const { return _img; }

    virtual unsigned int usedMem() const;

protected:
    ~ImageResource();

    Image *_img;
};

#endif

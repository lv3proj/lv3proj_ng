#include "ImageResource.h"
#include "io/image.h"

ImageResource::ImageResource(const char *name, Image *img)
 : Resource(name, RESOURCE_IMAGE)
 , _img(img)
{
}

ImageResource::~ImageResource()
{
    delete _img;
}

unsigned int ImageResource::usedMem() const
{
    return _img->w() * _img->h() * sizeof(Image::Pixel);
}

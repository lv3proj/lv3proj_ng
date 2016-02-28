#ifndef TEXTURE_BASE_H
#define TEXTURE_BASE_H

#include "ImageResource.h"

class Texture : public Resource
{
protected:
    Texture(ImageResource *img, unsigned id);
    virtual ~Texture();

public:

    inline unsigned getID() const { return id; }
    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    inline float getHalfWidth() const { return halfWidth; }
    inline float getHalfHeight() const { return halfHeight; }
    inline const Image *getSourceImage() const { return _srcImg.content()->getImage(); }

protected:
    unsigned id;

    int width, height;
    float halfWidth, halfHeight;
    CountedPtr<ImageResource> _srcImg;
};

#endif

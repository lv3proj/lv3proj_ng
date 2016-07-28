#include "Texture.h"
#include "io/image.h"

Texture::Texture(ImageResource *img)
 : Resource(img->name(), RESOURCE_TEXTURE)
 , id(0)
 , width(img->getImage()->w())
 , height(img->getImage()->h())
 , halfWidth(img->getImage()->w() / 2.0f)
 , halfHeight(img->getImage()->h() / 2.0f)
 , _srcImg(img)
{
}

Texture::~Texture()
{
}

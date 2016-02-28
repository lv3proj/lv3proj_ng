#include "Texture.h"

Texture::Texture(ImageResource *img, unsigned texID)
 : Resource(img->name(), RESOURCE_TEXTURE)
 , id(texID)
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

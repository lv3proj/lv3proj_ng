#include "Texture.h"

Texture *Texture::s_lastApplied = NULL;

Texture::Texture(const char *name)
 : Resource(name, RESOURCE_TEXTURE)
 , width(0)
 , height(0)
 , halfWidth(0)
 , halfHeight(0)
{
}

Texture::~Texture()
{
}

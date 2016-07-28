#include "Texture.h"
#include "ImageResource.h"
#include "io/image.h"
#include "glapi.h"

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::upload()
{
    if(id)
    {
        glDeleteTextures(1, &id);
        id = 0;
    }

    glGenTextures(1, &id);
    if(!id)
        return false;

    GLint pack, unpack;
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_BYTE, _srcImg->getImage()->data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

    return true;
}


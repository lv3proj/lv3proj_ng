#include "common.h"
#include "GLTexture.h"
#include <SDL/SDL.h>
#include "SDL_func.h"
#include "GL/gl.h"
#include "SDLSurfaceResource.h"
#include "ResourceMgr.h"


GLTexture::GLTexture(const char *name)
 : Texture(name)
 , _texID(0)
{
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &_texID);
}

void GLTexture::doApply()
{
    GLuint tex = _texID;
    glBindTexture(GL_TEXTURE_2D, tex);

    // FIXME
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool GLTexture::reload()
{
    SDLSurfaceResource *res = resMgr.LoadImg(name());

    // Conversion to 32-bit RGBA // TODO: fix for big endian
    SDL_Surface *src = res->getSurface();
    SDL_PixelFormat fmt;
    memset(&fmt, 0, sizeof(SDL_PixelFormat));
    fmt.BitsPerPixel = 32;
    fmt.BytesPerPixel = 4;
    fmt.alpha = 0xff;
    fmt.Rmask = 0x000000ff; // TODO: fix for big endian
    fmt.Gmask = 0x0000ff00;
    fmt.Bmask = 0x00ff0000;
    fmt.Amask = 0xff000000;

    // TODO: expand to power of 2 texture size

    uint32 w = src->w;
    uint32 h = src->h;

    // First, convert to 32 bpp RGBA-texture
    SDL_Surface *converted = SDL_ConvertSurface(src, &fmt, 0);
    if(!converted)
    {
        logerror("Failed to convert surface to OpenGL texture: %s", res->name());
        return false;
    }

    res->decref();

    // Then, transfer that to the final format.
    // This step seems necessary, because omitting it
    // resulted in left out pixels and the like, when testing.
    uint32 *raw = new uint32[w * h];
    uint32 *ptr = raw;

    // FIXME: this is *VERY* slow and could be done better.
    for(uint32 y = 0; y < h; ++y)
        for(uint32 x = 0; x < w; ++x)
            *ptr++ = SDLfunc_getpixel(converted, x, y);

    SDL_FreeSurface(converted);

    GLint pack, unpack;
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // No mipmapping for now.
    // TODO: Maybe later?
    glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw);

    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

    delete [] raw;

    setID(texId);
    this->width = w;
    this->height = h;

    return true;
}


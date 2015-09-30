#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Texture.h"

class GLTexture : public Texture
{
    friend class Renderer;

public:
    GLTexture(const char *);
    void setID(unsigned int id) { _texID = id; }

    virtual bool reload();

protected:
    ~GLTexture();
    void doApply() const;
    unsigned int _texID; // for OpenGL
};


#endif

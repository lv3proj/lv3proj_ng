#ifndef QUAD_H
#define QUAD_H

#include "RenderObject.h"
#include <string>

class Texture;

class Quad : public RenderObject
{
public:
    static Quad *create(const char *tex = NULL, int w = 0, int h = 0);
    virtual void destroy();

    // DO NOT USE - only used by memory system
    virtual ~Quad();

    bool setTexture(const char *tex);
    inline Texture *getTexture() const { return _texture; }

    // FIXME: This should be part of the texture class
    Vector upperLeftTextureCoordinates;
    Vector lowerRightTextureCoordinates;

protected:
    Quad(const char *tex = NULL, int w = 0, int h = 0);


    virtual void onRender() const;

    Texture *_texture;

};

#endif
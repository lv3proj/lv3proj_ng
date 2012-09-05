#ifndef QUAD_H
#define QUAD_H

#include "RenderObject.h"
#include <string>

class Texture;

class Quad : public RenderObject
{
public:
    Quad(const char *tex = NULL, int w = 0, int h = 0);
    virtual ~Quad();



    bool setTexture(const char *tex);
    inline Texture *getTexture() const { return _texture; }

    Vector upperLeftTextureCoordinates;
    Vector lowerRightTextureCoordinates;

protected:

    virtual void onRender() const;

    Texture *_texture;

};

#endif
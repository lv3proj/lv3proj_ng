#ifndef QUAD_H
#define QUAD_H

#include "RenderObject.h"
#include <string>

class Texture;

class Quad : public RenderObject
{
public:
    Quad();
    Quad(const char *tex, int w = -1, int h = -1);
    virtual ~Quad();

    int width;
    int height;

    inline Texture *getTexture() const { return _texture; }
    Vector upperLeftTextureCoordinates;
    Vector lowerRightTextureCoordinates;

protected:

    virtual void onRender() const;

    bool setTexture(const char *tex);

    Texture *_texture;

};

#endif
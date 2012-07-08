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

    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    inline float getHalfWidth() const { return halfWidth; }
    inline float getHalfHeight() const { return halfHeight; }
    void setWH(float w, float h);

    Vector upperLeftTextureCoordinates;
    Vector lowerRightTextureCoordinates;

protected:

    int width;
    int height;
    float halfWidth;
    float halfHeight;

    virtual void onRender() const;

    Texture *_texture;

};

#endif
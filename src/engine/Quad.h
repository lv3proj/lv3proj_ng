#ifndef QUAD_H
#define QUAD_H

#include "RenderObject.h"
#include <string>

class Texture;


class Quad : public RenderObject
{
public:
    static Quad *create();
    virtual void destroy();

    // DO NOT USE - only used by memory system
    virtual ~Quad();

    bool setTexture(const char *tex, const Rect *bound = NULL);
    inline Texture *getTexture() const { return _texture; }

    UV upperLeftTextureCoords;
    UV lowerRightTextureCoords;

protected:
    Quad();


    virtual void onRender() const;

    Texture *_texture;

};

#endif

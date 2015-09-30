#ifndef QUAD_H
#define QUAD_H

#include "RenderObject.h"
#include "refcounted.h"
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
    inline const Texture *getTexture() const { return _texture.content(); }

    UV upperLeftTextureCoords;
    UV lowerRightTextureCoords;

protected:
    Quad();


    virtual void onRender() const;

    CountedPtr<Texture> _texture;

};

#endif

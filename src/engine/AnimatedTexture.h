#ifndef ANIMATED_TEXTURE_H
#define ANIMATED_TEXTURE_H

#include "Texture.h"

class AnimatedTexture : public Texture
{
public:

    AnimatedTexture(const char *n);

protected:
    virtual ~AnimatedTexture();
    virtual void doApply();

public:

    virtual void update(float dt);
    virtual bool reload();

};


#endif


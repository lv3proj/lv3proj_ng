#include "AnimatedTexture.h"

AnimatedTexture::AnimatedTexture(const char *n)
 : Texture(n)
{
}

AnimatedTexture::~AnimatedTexture()
{
    // TODO: decref dep textures
}

void AnimatedTexture::doApply()
{
    // FIXME: write me
}

void AnimatedTexture::update(float dt)
{
    // FIXME: write me
}

bool AnimatedTexture::reload()
{
    return true;
}

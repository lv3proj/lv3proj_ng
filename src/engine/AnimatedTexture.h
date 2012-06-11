#ifndef ANIMATED_TEXTURE_H
#define ANIMATED_TEXTURE_H

#include "Texture.h"
#include "Anim.h"
#include "IntervalTimer.h"


class AnimatedTexture : public Texture
{
public:

    AnimatedTexture(Anim *);
    virtual void update(float dt);
    virtual bool reload();

protected:
    virtual ~AnimatedTexture();
    virtual void doApply();

    void _InitAnim();
    void _NextFrame();

    void SetFrameID(unsigned int id);
    void SetFrameName(const char *name);

    Anim *_ani;
    Texture *_activeTex; // currently active texture, changed in update()
    std::string _curName; // animation part name, in [] in the .anim file.
    AnimFrameStore *_curFrameStore; // holds all frames to the name above.
    AnimFrame *_curFrame; // "line" in the .anim file
    unsigned int _curFrameIdx;

    IntervalTimer _timer;
};


#endif


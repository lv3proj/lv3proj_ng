#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "common.h"
#include "Vector.h"

enum BlendType
{
    BLEND_NONE,
    BLEND_DEFAULT,
    BLEND_ADD,
    BLEND_SUB,
    BLEND_MULT,
};

class RenderObject
{
protected:
    RenderObject();

public:
    ~RenderObject();

    void update(float dt);

    InterpolatedVector position;
    InterpolatedVector offset;

    InterpolatedVector scale;

    InterpolatedVector color;

    InterpolatedVector alpha;
    InterpolatedVector alpha2;

    InterpolatedVector rotation;
    InterpolatedVector rotation2;

    InterpolatedVector velocity;
    InterpolatedVector gravity;


    virtual void onRender() const {}

    inline BlendType getBlendType() const { return _blend; }


protected:

    virtual void onUpdate(float dt) {};

    BlendType _blend;
};

#endif

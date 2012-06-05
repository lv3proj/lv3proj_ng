#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ScriptObject.h"
#include "Vector.h"

enum BlendType
{
    BLEND_NONE,
    BLEND_DEFAULT,
    BLEND_ADD,
    BLEND_SUB,
    BLEND_MULT,
};

class RenderObject : public ScriptObject
{
protected:
    RenderObject();

public:
    ~RenderObject();

    virtual void update(float dt);

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

    virtual void onEndOfLife();

    BlendType _blend;

};




#endif

#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ScriptObject.h"
#include "Vector.h"

class RenderLayer;

enum BlendType
{
    BLEND_NONE,
    BLEND_DEFAULT,
    BLEND_ADD,
    BLEND_SUB,
    BLEND_MULT,
};

enum LayerID
{
    LR_INVALID = 0,
};

class RenderObject : public LifeObject
{
    friend class RenderLayer;

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

    // scroll factor, 1 = stay at same position relative to camera (like GUI elements that never move)
    // 0.5 = scroll half as fast as the camera
    // 0 = scroll with the camera (for world objects)
    Vector parallax;


    virtual void onRender() const {}

    inline BlendType getBlendType() const { return _blend; }
    inline void setBlendType(BlendType b) {_blend = b; }

    inline unsigned int getLayer() const { return _layer; }

    void toLayer(unsigned int target);
    void moveToFront();
    void moveToBack();

    inline Vector getParallaxRenderPosition(const Vector& refPoint) const;


protected:

    virtual void onEndOfLife();

    BlendType _blend;
    unsigned int _layer;

};


Vector RenderObject::getParallaxRenderPosition(const Vector& refPoint) const
{
    Vector pos = position;
    pos += offset;

    pos -= refPoint;
    pos *= parallax;
    pos += refPoint;
    return pos;
}

#endif

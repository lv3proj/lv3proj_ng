#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ScriptObject.h"
#include "Vector.h"
#include "collision/Collidable.h"
#include "RenderLayer.h"
#include <set>

class RenderLayer;
class Collidable;

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

    typedef std::set<RenderObject*> Children;


    ~RenderObject();

    virtual void update(float dt);

    InterpolatedVector position;
    InterpolatedVector offset;

    InterpolatedVector scale;

    InterpolatedVector color;
    InterpolatedVector color2;

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

    Collidable *collider;

    virtual void onRender() const {}

    inline BlendType getBlendType() const { return _blend; }
    inline void setBlendType(BlendType b) {_blend = b; }

    inline unsigned int getLayer() const { return _layer; }
    inline RenderObject *getParent() const { return _parent; }

    void toLayer(unsigned int target);
    void moveToFront();
    void moveToBack();

    void addChild(RenderObject *);
    void removeChild(RenderObject *);
    inline const Children& getChildren() const { return _children; }

    inline Vector getParallaxRenderPosition(const Vector& refPoint) const;
    inline Vector getParallaxFactor() const;

    inline bool collidesWith(const RenderObject *other, Vector *result);

    bool isVisible() const;
    Vector getAbsolutePosition() const; // for UI elements

protected:

    virtual void onEndOfLife();

    BlendType _blend;
    unsigned int _layer;
    RenderObject *_parent;
    RenderLayer *_layerPtr;

private:

    Children _children;

};


Vector RenderObject::getParallaxRenderPosition(const Vector& refPoint) const
{
    Vector pos = position;
    //pos += offset;

    pos -= refPoint;
    pos *= getParallaxFactor();
    pos += refPoint;
    //return pos;
    return pos + offset;
}

Vector RenderObject::getParallaxFactor() const
{
    if(_layerPtr->parallax.isZero())
        return parallax;
    return parallax * _layerPtr->parallax;
}

bool RenderObject::collidesWith(const RenderObject *other, Vector *result)
{
    return collider && collider->collidesWith(other->collider, result);
}

#endif

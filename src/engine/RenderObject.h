#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "ScriptObject.h"
#include "Vector.h"
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

    typedef std::set<RenderObject*> Children; // TODO: make this vector


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
    InterpolatedVector friction;

    // scroll factor, 1 = stay at same position relative to camera (like GUI elements that never move)
    // 0.5 = scroll half as fast as the camera
    // 0 = scroll with the camera (for world objects)
    Vector parallax;

    virtual void onRender() const {}

    virtual void updatePhysics(float dt);

    inline BlendType getBlendType() const { return _blend; }
    inline void setBlendType(BlendType b) {_blend = b; }

    inline RenderLayer *getLayerPtr() const { return _layerPtr; }

    inline RenderObject *getParent() const { return _parent; }

    void toLayer(unsigned int target);
    void moveToFront();
    void moveToBack();

    void addChild(RenderObject *);
    void removeChild(RenderObject *);
    inline const Children& getChildren() const { return _children; }

    inline Vector getParallaxRenderPosition(const Vector& refPoint) const;
    inline Vector getParallaxFactor() const;
    bool isOnScreen() const;

    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }
    inline float getHalfWidth() const { return halfWidth; }
    inline float getHalfHeight() const { return halfHeight; }
    inline void setWH(float w, float h);

    bool isVisible() const;
    Vector getAbsolutePosition() const; // for UI elements

    static float s_cullX1, s_cullX2, s_cullY1, s_cullY2;

    RenderLayer::Block *_layerBlock;
    unsigned int _indexInBlock;

protected:

    virtual void onEndOfLife();

    // Special update function, per object type
    // automatically called in update()
    virtual void onUpdate(float dt);

    int width;
    int height;
    float halfWidth;
    float halfHeight;

    BlendType _blend;
    RenderObject *_parent;
    RenderLayer *_layerPtr;
    bool _noCull;

private:

    Children _children;

};

void RenderObject::setWH(float w, float h)
{
    width = w;
    height = h;
    halfWidth =  w / 2.0f;
    halfHeight = h / 2.0f;
}


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
    /*if(_layerPtr->parallax.isZero())
        return parallax;
    return parallax * _layerPtr->parallax;*/

    if(parallax.isZero())
        return _layerPtr->parallax;
    return parallax;
}

#endif

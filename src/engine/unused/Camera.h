#ifndef CAMERA_H
#define CAMERA_H

#include "RenderObject.h"

class Camera : public RenderObject
{
public:
    Camera();
    virtual ~Camera();

    virtual void update(float dt);

    Vector ToWorldPosition(const Vector& v) const;
    Vector ToWindowPosition(const Vector& v) const;
    Vector ToWorldScale(const Vector& v) const;
    Vector ToWindowScale(const Vector& v) const;

    Vector invScale;
    Vector screenCenter; // world position where the center of the screen is at
    float virtualW;
    float virtualH;
};


#endif

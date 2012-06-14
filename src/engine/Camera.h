#ifndef CAMERA_H
#define CAMERA_H

#include "RenderObject.h"

class Camera : public RenderObject
{
public:
    Camera();
    virtual ~Camera();

    virtual void update(float dt);

    Vector invScale;
};


#endif

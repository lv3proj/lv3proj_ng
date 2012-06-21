#include "Camera.h"

Camera::Camera()
{
    scale = Vector(1, 1, 1);
    invScale = Vector(1, 1, 1);
}

Camera::~Camera()
{
}

void Camera::update(float dt)
{
    if(dt)
        RenderObject::update(dt);

    invScale.x = 1 / scale.x;
    invScale.y = 1 / scale.y;
}

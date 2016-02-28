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

    screenCenter.x = position.x + (virtualW * 0.5f) * invScale.x;
    screenCenter.y = position.y + (virtualH * 0.5f) * invScale.y;
}

Vector Camera::ToWorldPosition(const Vector& v) const
{
    Vector ret = v;
    ret.x -= (virtualW * 0.5f);
    ret.y -= (virtualH * 0.5f);
    ret *= invScale;
    ret += screenCenter;
    return ret;
}

Vector Camera::ToWindowPosition(const Vector& v) const
{
    Vector ret = v;
    ret -= screenCenter;
    ret *= scale;
    ret.x += (virtualW * 0.5f);
    ret.y += (virtualH * 0.5f);
    return ret;
}

Vector Camera::ToWorldScale(const Vector& v) const
{
    return v * scale;
}

Vector Camera::ToWindowScale(const Vector& v) const
{
    return v * invScale;
}

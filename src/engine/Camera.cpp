#include "Camera.h"
#include "Engine.h"
#include "Renderer.h"

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

    // FIXME: get settings from engine, not renderer
    const RenderSettings& rr = engine->GetRenderer()->getSettings();
    screenCenter.x = position.x + (rr.virtualW / 2) * invScale.x;
    screenCenter.y = position.y + (rr.virtualH / 2) * invScale.y;
}

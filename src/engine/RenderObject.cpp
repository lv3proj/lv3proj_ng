#include "RenderObject.h"


RenderObject::RenderObject()
{
    _blend = BLEND_DEFAULT;
    scale = Vector(1, 1, 1);
    color = Vector(1, 1, 1);
    alpha = 1;
    alpha2 = 1;
}

RenderObject::~RenderObject()
{
}

void RenderObject::update(float dt)
{
    position.update(dt);
    offset.update(dt);
    scale.update(dt);
    color.update(dt);
    alpha.update(dt);
    alpha2.update(dt);
    rotation.update(dt);
    rotation2.update(dt);
    velocity.update(dt);
    gravity.update(dt);

    onUpdate(dt);
}

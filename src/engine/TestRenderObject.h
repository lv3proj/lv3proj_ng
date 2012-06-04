#ifndef TEST_RENDEROBJECT_H
#define TEST_RENDEROBJECT_H

#include "Quad.h"

class TestRenderObject : public Quad
{
public:
    TestRenderObject();
    virtual ~TestRenderObject();

    virtual void onUpdate(float dt);
    virtual void onRender() const;

protected:

    bool loadtex;
};

#endif

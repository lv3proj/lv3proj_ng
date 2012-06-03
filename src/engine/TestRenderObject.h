#ifndef TEST_RENDEROBJECT_H
#define TEST_RENDEROBJECT_H

#include "RenderObject.h"

class TestRenderObject : public RenderObject
{
public:
    TestRenderObject();
    virtual ~TestRenderObject();

protected:

    void onRender();

};

#endif

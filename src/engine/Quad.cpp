#include "Quad.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Arenas.h"
#include "ObjectMgr.h"
#include "ResourceMgr.h"

namespace Arenas {
    Arenas::QuadMem quadArena(Arenas::chunkAlloc, 1000, sizeof(Quad) );
};

Quad *Quad::create()
{
    Quad *q = XNEW(Quad, Arenas::quadArena)();
    g_engine->objmgr->AddObject(q);
    return q;
}

void Quad::destroy()
{
    ASSERT_DYNAMIC_TYPE(*this, Quad);
    return XDELETE_NN(this, Arenas::quadArena);
}


Quad::Quad()
: _texture(NULL)
{
    setWH(0, 0);
}

Quad::~Quad()
{
}


bool Quad::setTexture(const char *tex, const Rect *bound /* = NULL */)
{
    CountedPtr<Texture> newtex = g_engine->resmgr->getTex(tex); // increases refcount
    if(!newtex)
        return false;

    const float tw = float(newtex->getWidth());
    const float th = float(newtex->getHeight());
    if(bound)
    {
        upperLeftTextureCoords  = UV(bound->x / tw, bound->y / th);
        lowerRightTextureCoords = UV((bound->x + bound->w) / tw, (bound->y + bound->h) / th);
        setWH(bound->w, bound->h);
    }
    else
    {
        upperLeftTextureCoords = UV(0, 0);
        lowerRightTextureCoords = UV(1, 1);
        setWH(tw, th);
    }

    _texture = newtex;

    return true;
}

void Quad::onRender() const
{
    ASSERT(false);
    //g_engine->render->renderQuad(this);
}


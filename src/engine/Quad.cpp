#include "Quad.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Arenas.h"
#include "ObjectMgr.h"

namespace Arenas {
    Arenas::QuadMem quadArena(Arenas::chunkAlloc, 1000, sizeof(Quad) );
};

Quad *Quad::create()
{
    Quad *q = XNEW(Quad, Arenas::quadArena)();
    engine->objmgr->AddObject(q);
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
    if(_texture)
        _texture->decref();
}


bool Quad::setTexture(const char *tex, const Rect *bound /* = NULL */)
{
    // Not necessary to set texture if already set to same
    if(tex && _texture && !strcmp(tex, _texture->name()))
        return true;

    Texture *newtex = engine->GetTexture(tex); // increases refcount
    if(!newtex)
        return false;

    if(_texture)
        _texture->decref();

    _texture = newtex;
    setWH(newtex->getWidth(), newtex->getHeight());

    if(bound)
    {
        upperLeftTextureCoordinates.u = (float)(int)bound->x / (float)(int)newtex->getWidth();
        upperLeftTextureCoordinates.v = (float)(int)(bound->x + bound->w) / (float)(int)newtex->getWidth();
        lowerRightTextureCoordinates.u = (float)(int)bound->y / (float)(int)newtex->getHeight();
        lowerRightTextureCoordinates.v = (float)(int)(bound->y + bound->h) / (float)(int)newtex->getHeight();
    }

    return true;
}

void Quad::onRender() const
{
    engine->GetRenderer()->renderQuad(this);
}


#include "Quad.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"


Quad::Quad(const char *tex /* = NULL */, int w /* = 0 */, int h /* = 0 */)
 : upperLeftTextureCoordinates(0, 0)
 , lowerRightTextureCoordinates(1, 1)
 , _texture(NULL)
 , width(0)
 , height(0)
 , halfWidth(0)
 , halfHeight(0)
{
    if(tex && *tex)
        setTexture(tex);
    if(w > 0)
    {
        width = w;
        if(h > 0)
            height = h;
        else
            height = w;
    }
}

Quad::~Quad()
{
    if(_texture)
        _texture->decref();
}

bool Quad::setTexture(const char *tex)
{
    // Not necessary to set texture if already set to same
    if(_texture && !strcmp(tex, _texture->name()))
        return true;

    Texture *newtex = engine->GetTexture(tex); // increases refcount
    if(!newtex)
        return false;

    if(_texture)
        _texture->decref();

    _texture = newtex;
    setWH(newtex->getWidth(), newtex->getHeight());
    return true;
}

void Quad::onRender() const
{
    engine->GetRenderer()->renderQuad(this);
}

void Quad::setWH(float w, float h)
{
    width = w;
    height = h;
    halfWidth =  w / 2.0f;
    halfHeight = h / 2.0f;
}


#include "Quad.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"

Quad::Quad()
 : _texture(NULL)
 , width(0)
 , height(0)
 , upperLeftTextureCoordinates(0, 0)
 , lowerRightTextureCoordinates(1, 1)
{
}

Quad::~Quad()
{
    if(_texture)
        _texture->decref();
}

Quad::Quad(const char *tex, int w /* = -1 */, int h /* = -1 */)
{
    setTexture(tex);
    if(w >= 0)
    {
        width = w;
        if(h >= 0)
            height = h;
        else
            height = w;
    }
}

bool Quad::setTexture(const char *tex)
{
    Texture *newtex = engine->GetTexture(tex); // increases refcount
    if(!newtex)
        return false;

    if(_texture)
        _texture->decref();

    _texture = newtex;
    width = newtex->getWidth();
    height = newtex->getHeight();
    return true;
}

void Quad::onRender() const
{
    engine->GetRenderer()->renderQuad(this);
}

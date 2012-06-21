#ifndef OBSRENDER_H
#define OBSRENDER_H

// For debugging mostly

#include "RenderObject.h"

class Texture;
class Tile;

class ObsRender : public RenderObject
{
public:
    ObsRender(unsigned int layer);
    virtual ~ObsRender();

    virtual void onRender() const;

protected:
    unsigned int _layer;
    Texture *_tex;

private:
    void _renderMixedTile(Tile *tile, float xpos, float ypos) const;
};

#endif

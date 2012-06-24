#ifndef OBSRENDER_H
#define OBSRENDER_H

// For debugging mostly

#include "RenderObject.h"

class Texture;
class Tile;

// Renders obstruction for one layer
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


// Renders obstruction for the global ObsGrid

class ObsGridRender : public RenderObject
{
public:
    ObsGridRender();
    virtual ~ObsGridRender();

    virtual void onRender() const;

protected:
    Texture *_tex;

private:
    void _renderMixedBlock(const unsigned char *tile, float xpos, float ypos) const;
};

#endif

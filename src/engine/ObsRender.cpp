#include "ObsRender.h"
#include "Engine.h"
#include "Renderer.h"
#include "TileGrid.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"

ObsRender::ObsRender(unsigned int layer)
 : _layer(layer)
{
    _tex = engine->GetTexture("");
}

ObsRender::~ObsRender()
{
}

void ObsRender::onRender() const
{
    TileGrid& grid = engine->layers->GetLayer(_layer)->tiles;
    Renderer *render = engine->GetRenderer();

    const unsigned int dim = grid.GetSize();
    for(unsigned int y = 0; y < dim; ++y)
    {
        for(unsigned int x = 0; x < dim; ++x)
        {
            if(Tile *tile = grid.GetTile(x, y))
            {
                const unsigned int tdim = tile->getSize();
                const float tdimfh = tdim / 2.0f;
                switch(tile->getTileObsType())
                {
                    case TO_FULLFREE:
                        break;

                    case TO_MIXED:
                        _renderMixedTile(tile, x * tdim - tdimfh + 0.5f, y * tdim - tdimfh + 0.5f);
                        break;

                    case TO_FULLSOLID:
                        render->renderSingleTexture(_tex,
                            Vector(x * tdim, y * tdim),
                            Vector(tdimfh, tdimfh));
                        break;
                }
            }
        }
    }
}

void ObsRender::_renderMixedTile(Tile *tile, float xpos, float ypos) const
{
    const int tdim = tile->getSize();
    Renderer *render = engine->GetRenderer();
    Texture **texptrs = (Texture**)alloca(tdim * sizeof(Texture*));

    for(int py = 0; py < tdim; ++py)
    {
        const unsigned char *obsptr = &(tile->getObsMask()(0, py));
        for(int px = 0; px < tdim; ++px)
        {
            texptrs[px] = obsptr[px] ? _tex : NULL;
        }

        render->renderTextureArray(texptrs, tdim, Vector(xpos, ypos + py), Vector(1, 0), Vector(0.5f, 0.5f));
    }
}


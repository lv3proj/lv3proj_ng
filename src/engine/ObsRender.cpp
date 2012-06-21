#include "ObsRender.h"
#include "Engine.h"
#include "Renderer.h"
#include "TileGrid.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"

ObsRender::ObsRender(unsigned int layer)
 : _layer(layer)
{
    _tex = engine->GetTexture("obs.png");
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
                    case TO_MIXED:
                    {
                        for(unsigned int py = 0; py < tdim; ++py)
                            for(unsigned int px = 0; px < tdim; ++px)
                                if(tile->isObs(px, py))
                                    render->renderSingleTexture(_tex,
                                        Vector(x + px, y + py),
                                        Vector(0.5f, 0.5f));
                        break;
                    }

                    case TO_FULLSOLID:
                        render->renderSingleTexture(_tex,
                            Vector(x * tdim, y * tdim),
                            Vector(tdimfh, tdimfh));
                        break;

                    case TO_FULLFREE:
                        break;
                }
            }
        }
    }
}

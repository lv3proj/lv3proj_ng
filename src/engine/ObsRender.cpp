#include "ObsRender.h"
#include "Engine.h"
#include "Renderer.h"
#include "TileGrid.h"
#include "RenderLayerMgr.h"
#include "RenderLayer.h"
#include "Texture.h"
#include "ObsGrid.h"


ObsRender::ObsRender(unsigned int layer)
 : _layer(layer)
{
    _tex = engine->GetTexture("");
    _noCull = true;
}

ObsRender::~ObsRender()
{
}

void ObsRender::onRender() const
{
    TileGrid& grid = *(engine->layers->GetLayer(_layer)->tiles);

    int xstart, ystart, xend, yend;
    grid.CalcRenderLimits(xstart, ystart, xend, yend);
    if((xend - xstart) <= 0 || (yend - ystart) <= 0)
        return;

    Renderer *render = engine->GetRenderer();
    _tex->apply();

    float vertexData[8];
    const float ts = grid.GetTileSizef();

    for(int y = ystart; y < yend; ++y)
    {
        vertexData[1] = vertexData[3] = y * ts; // current row
        vertexData[5] = vertexData[7] = vertexData[1] + ts; // one block row below
        int c = 0; // how many consecutive obs found in one run
        for(int x = xstart; x < xend; ++x)
        {
            Tile *tile = grid.GetTile(x, y);
            if(tile)
            {
                switch(tile->getTileObsType())
                {
                    case TO_MIXED:
                        _renderMixedTile(tile, x * ts, y * ts);
                        // fall through

                    case TO_FULLFREE:
                        tile = NULL;
                        break;

                    case TO_FULLSOLID:
                        // Starting a new run? Remember current x pos.
                        if(!c)
                            vertexData[2] = vertexData[6] = x * ts;
                        ++c;
                        break;
                }
            }
            // tile == NULL serves as an indicator here that the tile was not fully solid.
            if(c && !tile) // Reached end of run?
            {
                vertexData[0] = vertexData[4] = vertexData[2] + (c * ts);  // where run started + c tiles.
                render->render2DVertexArray(&vertexData[0], 4);
                c = 0;
            }
        }
        if(c) // Reached end of run?
        {
            vertexData[0] = vertexData[4] = vertexData[2] + (c * ts);  // where run started + c pixels.
            render->render2DVertexArray(&vertexData[0], 4);
            c = 0;
        }
    }
}

void ObsRender::_renderMixedTile(Tile *tile, float xpos, float ypos) const
{
    const int tdim = tile->getSize();
    Renderer *render = engine->GetRenderer();

    float vertexData[8];

    for(int py = 0; py < tdim; ++py)
    {
        vertexData[1] = vertexData[3] = ypos + py; // current row
        vertexData[5] = vertexData[7] = vertexData[1] + 1.0f; // one pixel below
        int c = 0; // how many consecutive obs found in one run
        const unsigned char *obsptr = &(tile->getObsMask()(0, py));
        for(int px = 0; px < tdim; ++px)
        {
            if(obsptr[px])
            {
                // Starting a new run? Remember current x pos.
                if(!c)
                    vertexData[2] = vertexData[6] = xpos + px;
                ++c;
            }
            else if(c)
            {
                // Reached end of run.
                vertexData[0] = vertexData[4] = vertexData[2] + float(c);  // where run started + c pixels.
                render->render2DVertexArray(&vertexData[0], 4);
                c = 0;
            }
        }
        // Unfinished run? Finish.
        if(c)
        {
            vertexData[0] = vertexData[4] = vertexData[2] + float(c); // where run started + c pixels.
            render->render2DVertexArray(&vertexData[0], 4);
            c = 0;
        }
    }
}

//-----------------------------------------------------------------------





ObsGridRender::ObsGridRender()
{
    _tex = engine->GetTexture("");
    _noCull = true;
}

ObsGridRender::~ObsGridRender()
{
}

void ObsGridRender::onRender() const
{
    ObsGrid& ogrid = engine->obsgrid;
    const float ts = ogrid._blockdim;

    int xstart, ystart, xend, yend;
    engine->CalcRenderLimits(ogrid._grid.size1d(), ts, xstart, ystart, xend, yend);
    if((xend - xstart) <= 0 || (yend - ystart) <= 0)
        return;

    Renderer *render = engine->GetRenderer();
    _tex->apply();

    float vertexData[8];

    for(int y = ystart; y < yend; ++y)
    {
        vertexData[1] = vertexData[3] = y * ts; // current row
        vertexData[5] = vertexData[7] = vertexData[1] + ts; // one block row below
        int c = 0; // how many consecutive obs found in one run
        for(int x = xstart; x < xend; ++x)
        {
            const ObsGrid::mask *block = ogrid._grid(x, y);
            if(block == ogrid._full)
            {
                // Starting a new run? Remember current x pos.
                if(!c)
                    vertexData[2] = vertexData[6] = x * ts;
                ++c;
                continue;
            }
            else if(block == ogrid._empty)
            {
                block = NULL;
            }
            else
            {
                _renderMixedBlock(block, x * ts, y * ts);
                block = NULL;
            }

            // tile == NULL serves as an indicator here that the tile was not fully solid.
            if(c && !block) // Reached end of run?
            {
                vertexData[0] = vertexData[4] = vertexData[2] + (c * ts);  // where run started + c tiles.
                render->render2DVertexArray(&vertexData[0], 4);
                c = 0;
            }
        }
        if(c) // Reached end of run?
        {
            vertexData[0] = vertexData[4] = vertexData[2] + (c * ts);  // where run started + c pixels.
            render->render2DVertexArray(&vertexData[0], 4);
            c = 0;
        }
    }
}

void ObsGridRender::_renderMixedBlock(const unsigned char *block, float xpos, float ypos) const
{
    const int tdim = engine->obsgrid._blockdim;
    Renderer *render = engine->GetRenderer();

    float vertexData[8];

    for(int py = 0; py < tdim; ++py)
    {
        vertexData[1] = vertexData[3] = ypos + py; // current row
        vertexData[5] = vertexData[7] = vertexData[1] + 1.0f; // one pixel below
        int c = 0; // how many consecutive obs found in one run
        const unsigned char *obsptr = &block[py * tdim];
        for(int px = 0; px < tdim; ++px)
        {
            if(obsptr[px])
            {
                // Starting a new run? Remember current x pos.
                if(!c)
                    vertexData[2] = vertexData[6] = xpos + px;
                ++c;
            }
            else if(c)
            {
                // Reached end of run.
                vertexData[0] = vertexData[4] = vertexData[2] + float(c);  // where run started + c pixels.
                render->render2DVertexArray(&vertexData[0], 4);
                c = 0;
            }
        }
        // Unfinished run? Finish.
        if(c)
        {
            vertexData[0] = vertexData[4] = vertexData[2] + float(c); // where run started + c pixels.
            render->render2DVertexArray(&vertexData[0], 4);
            c = 0;
        }
    }
}


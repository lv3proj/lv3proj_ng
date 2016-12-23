#include "TileMapRender.h"
#include "opengl/glapi.h"

void TileMapRender::render() const
{
    if(const size_t sz = tilemap->rdata.size())
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, tilemap->vbo.id());
        for(size_t i = 0; i < sz; ++i)
        {
            const TileMap::RenderData& rd = tilemap->rdata[i];
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, rd.ibo.id());
            glBindTexture(GL_TEXTURE_2D, rd.tex->getID());
            glDrawElements(GL_TRIANGLES, rd.ibo.used, GL_UNSIGNED_INT, NULL);
        }
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
}


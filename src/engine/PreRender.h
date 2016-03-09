#ifndef PRERENDER_H
#define PRERENDER_H

#include "glm/glm.hpp"
#include <vector>

class BaseObject;
class Sprite;
class GroupObject;

class RenderLayer;



enum VertexMode
{
    VERTMODE_POINTS,
    VERTMODE_LINES,
    VERTMODE_TRIANGLES,
    VERTMODE_TRIANGLE_STRIP,
};

// This is what the renderer takes
struct RenderInfo
{
    void *ppvert2d;
    void *ppuv;

    void *pcolor;
    void *pnumVert;
    VertexMode *pvertmode;
    void *ptransform;
    void *ptex;
};


struct RenderBuffer
{
    std::vector<const glm::vec2*> pvert2d;  // pointer to this object's N verts 
    std::vector<const glm::vec2*> puv;      // pointer to this object's N UV coords

    std::vector<glm::vec4> color;     // 1 color per object
    std::vector<unsigned> numVert;    // 1 number of verts for this obj (N = num[objIdx])
    std::vector<VertexMode> vertMode; // 1 vertex render mode per object
    std::vector<glm::mat4> transform; // 1 transform matrix per object
    std::vector<unsigned> texID;      // 1 tex ID per object
};

class PreRender
{
    size_t addObj(const glm::mat4& proj, const BaseObject *obj);
    size_t addSprite(glm::mat4 proj, const Sprite *obj);
    size_t addGroup(glm::mat4 proj, const GroupObject *obj);
    void addLayer(const RenderLayer& lr);
    void begin();

protected:
    RenderBuffer buf;
};


#endif

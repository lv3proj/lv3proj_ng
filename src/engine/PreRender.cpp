#include "PreRender.h"
#include "Objects.h"
#include "common.h"
#include <glm/ext.hpp>

using namespace glm;

static const glm::vec2 spriteVertices[] =
{
    vec2(-1, -1), // upper left
    vec2(+1, -1), // upper right
    vec2(-1, +1), // lower left
    vec2(+1, +1), // lower right
};

void PreRender::begin()
{
    buf.color.clear();
    buf.numVert.clear();
    buf.puv.clear();
    buf.pvert2d.clear();
    buf.texID.clear();
    buf.transform.clear();
}

size_t PreRender::addObj(const mat4 &proj, const BaseObject *obj)
{
    switch(obj->getType())
    {
        case RO_SPRITE:
            return addSprite(proj, (Sprite*)obj);
        case RO_GROUP:
            return addGroup(proj, (GroupObject*)obj);
        default:
            ASSERT(false);
    }
    return 0;
}

size_t PreRender::addGroup(mat4 proj, const GroupObject *obj)
{
    const size_t sz = obj->size();
    size_t added = 0;
    if(sz)
    {
        // TODO FIXME
        //proj = glm::translate(proj, xx);
        //proj = glm::rotate(proj,  xx);

        for(size_t i = 0; i < sz; ++i)
            added += addObj(proj, obj->child(i));
    }
    return added;
}

size_t PreRender::addSprite(mat4 proj, const Sprite *obj)
{
    buf.transform.push_back(translate(proj, vec3(obj->pos, 0.0f)));
    buf.color.push_back(obj->color);
    buf.puv.push_back(&obj->uv[0]);
    buf.pvert2d.push_back(spriteVertices);
    buf.numVert.push_back(4);
    buf.vertMode.push_back(VERTMODE_TRIANGLE_STRIP);

    buf.texID.push_back(obj->tex->getID());
    return 1;
}
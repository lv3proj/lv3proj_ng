#include "Objects.h"
#include "common.h"
#include <algorithm>

BaseObject::BaseObject(ObjectType ty)
: _type(ty)
, _refcount(0)
{
}

BaseObject::~BaseObject()
{
}

void BaseObject::_Destroy(BaseObject *ro)
{
    switch(ro->getType())
    {
        case RO_VIRTUAL:
            VirtualRenderObject::_Destroy((VirtualRenderObject*)ro);
            break;
        case RO_GROUP:
            GroupObject::_Destroy((GroupObject*)ro);
            break;
        case RO_SPRITE:
            Sprite::_Destroy((Sprite*)ro);
            break;
        default:
            ASSERT(false);
    }
}

void BaseObject::decref()
{
    if(_refcount-- <= 1)
        _Destroy(this);
}

Sprite::Sprite() : BaseObject(RO_SPRITE)
{
}

Sprite::~Sprite()
{
}


Sprite *Sprite::Create()
{
    return new Sprite;
}

void Sprite::_Destroy(Sprite *sp)
{
    delete sp;
}

glm::mat4 Sprite::getLocalTransform() const
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
}

GroupObject::GroupObject() : BaseObject(RO_GROUP)
{
}

GroupObject::~GroupObject()
{
}


GroupObject *GroupObject::Create()
{
    return new GroupObject;
}

void GroupObject::_Destroy(GroupObject *grp)
{
    delete grp;
}

void GroupObject::remove(unsigned idx)
{
    const size_t sz = _ch.size();
    if(idx < sz)
        _ch.erase(_ch.begin() + idx);
}

unsigned GroupObject::remove(BaseObject *ro)
{
    const size_t oldsz = _ch.size();
    Children::iterator it = std::remove(_ch.begin(), _ch.end(), ro);
    const size_t newsz = it - _ch.begin();
    _ch.resize(newsz);
    return unsigned(oldsz - newsz);
}

glm::mat4 ObjectInterpolationData::getLocalTransform() const
{
    glm::mat4 m = glm::mat4(1);
    m = glm::translate(m, glm::vec3(position.value(), 0.0f));
    m = glm::rotate(m, rotation.value(), glm::vec3(0,0,1));
    return m;
}

void ObjectInterpolationData::update(float dt)
{
    position.update(dt);
    rotation.update(dt);
    scale.update(dt);
    color.update(dt);
}

Quad::Quad() : BaseObject(RO_QUAD)
{
}

Quad::~Quad()
{
}

Quad *Quad::Create()
{
    return new Quad;
}

void Quad::_Destroy(Quad *q)
{
    delete q;
}

VirtualRenderObject::~VirtualRenderObject()
{
}

void VirtualRenderObject::_Destroy(VirtualRenderObject *ro)
{
    delete ro;
}
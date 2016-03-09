#include "Objects.h"
#include "common.h"
#include <algorithm>

BaseObject::BaseObject(ObjectType ty)
: _type(ty)
, _refcount(0)
{
}

void BaseObject::_Destroy(BaseObject *ro)
{
    switch(ro->getType())
    {
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

Sprite *Sprite::Create()
{
    return new Sprite;
}

void Sprite::_Destroy(Sprite *sp)
{
    delete sp;
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


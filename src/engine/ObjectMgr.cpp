#include "ObjectMgr.h"
#include "ScriptObject.h"
#include "common.h"
#include <assert.h>

ObjectMgr::ObjectMgr()
{
    _alive.reserve(1000);
    _garbage.reserve(10);
}

ObjectMgr::~ObjectMgr()
{
    ClearAll();
}

void ObjectMgr::ClearAll()
{
    ClearGarbage();
    for(size_t i = 0; i < _alive.size(); ++i)
        if(_alive[i])
            _alive[i]->destroy();
    _alive.clear();
}

void ObjectMgr::Update(float dt)
{
    for(size_t i = 0; i < _alive.size(); ++i)
    {
        ScriptObject *obj = _alive[i];
        if(!(obj->isDead() || obj->isPaused()))
            obj->update(dt);
    }
}

void ObjectMgr::UpdateFixed(float dt)
{
    for(size_t i = 0; i < _alive.size(); ++i)
    {
        ScriptObject *obj = _alive[i];
        if(!(obj->isDead() || obj->isPaused()))
            obj->updateFixed(dt);
    }
}

void ObjectMgr::AddObject(ScriptObject *obj)
{
    ASSERT(obj->isManaged());
    obj->_objIndex = (unsigned int)_alive.size();
    _alive.push_back(obj);
}

void ObjectMgr::Garbage(ScriptObject *obj)
{
    ASSERT(obj->isManaged());
    _garbage.push_back(obj);
}

void ObjectMgr::ClearGarbage()
{
    ASSERT(_garbage.size() <= _alive.size());
    if(_garbage.size())
    {
        //logdev("ObjectMgr: Deleting %u objects", (unsigned int)_garbage.size());
        for(size_t i = 0; i < _garbage.size(); ++i)
        {
            ScriptObject *obj = _garbage[i];
            unsigned int idx = obj->_objIndex;
            ASSERT(_alive[idx] == obj);
            obj->destroy();

            obj = _alive.back(); // there is always at least 1 item left
            obj->_objIndex = idx;
            _alive[idx] = obj;
            _alive.pop_back();
        }
        _garbage.clear();
    }
}

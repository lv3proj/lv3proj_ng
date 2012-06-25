#include "ObjectMgr.h"
#include "ScriptObject.h"
#include "common.h"
#include <assert.h>

ObjectMgr::ObjectMgr()
{
}

ObjectMgr::~ObjectMgr()
{
    ClearAll();
}

void ObjectMgr::ClearAll()
{
    for(std::set<ScriptObject*>::iterator it = _alive.begin(); it != _alive.end(); ++it)
        _garbage.push_back(*it);
    _alive.clear();
    ClearGarbage();
}

void ObjectMgr::Update(float dt)
{
    for(std::set<ScriptObject*>::iterator it = _alive.begin(); it != _alive.end(); ++it)
    {
        if(!(*it)->isDead())
            (*it)->update(dt);
    }
}

void ObjectMgr::AddObject(ScriptObject *obj)
{
    assert(obj->isManaged());
    _alive.insert(obj);
}

void ObjectMgr::Garbage(ScriptObject *obj)
{
    assert(obj->isManaged());
    _garbage.push_back(obj);
}

void ObjectMgr::ClearGarbage()
{
    if(_garbage.size())
    {
        logdev("ObjectMgr: Deleting %u objects", (unsigned int)_garbage.size());
        for(size_t i = 0; i < _garbage.size(); ++i)
        {
            _alive.erase(_garbage[i]);
            delete _garbage[i];
        }
        _garbage.clear();
    }
}

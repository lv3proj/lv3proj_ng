#ifndef OBJECT_MGR_H
#define OBJECT_MGR_H

#include <vector>
#include <set>

class ScriptObject;

class ObjectMgr
{
public:
    ObjectMgr();
    ~ObjectMgr();
    void AddObject(ScriptObject *);
    void Garbage(ScriptObject *);
    void Update(float dt);
    void UpdateFixed(float dt);
    void ClearGarbage();
    void ClearAll();

private:
    typedef std::vector<ScriptObject*> ScriptObjectStore;
    ScriptObjectStore _alive;
    ScriptObjectStore _garbage;
};

#endif

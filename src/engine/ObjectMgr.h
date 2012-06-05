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
    void ClearGarbage();
    void ClearAll();

private:
    std::set<ScriptObject*> _alive;
    std::vector<ScriptObject*> _garbage;
};

#endif

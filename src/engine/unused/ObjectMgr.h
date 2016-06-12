#ifndef OBJECT_MGR_H
#define OBJECT_MGR_H

#include <vector>
#include <set>
#include "Objects.h"


class ObjectMgr
{
public:
    ObjectMgr();
    ~ObjectMgr();
    void AddObject(BaseObject *);
    void Update(float dt);
    void ClearGarbage();
    void ClearAll();

private:
    typedef std::vector<BaseObject*> ObjStore;
    ObjStore _bytype[RO_MAX];
};

#endif

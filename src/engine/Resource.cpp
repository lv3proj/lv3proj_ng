#include "Resource.h"
#include <assert.h>

Resource::Resource(const char *n, ResourceType t)
 : _name(n), _type(t), _refcount(1)
{
}

Resource::~Resource()
{
    for(std::list<Resource*>::iterator it = _dep.begin(); it != _dep.end(); ++it)
        (*it)->decref();
}

void Resource::decref()
{
    if(!_refcount)
    {
        assert(false);
    }

    --_refcount;

    // TODO: if required, add a flag for instant auto-deletion if necessary
    // but note that this should be used only for non-managed resources then
}

void Resource::addDep(Resource *r)
{
    r->incref();
    _dep.push_back(r);
}

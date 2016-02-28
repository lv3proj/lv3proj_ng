#include "Resource.h"
#include <assert.h>

Resource::Resource(const char *n, ResourceType t)
 : _type(t), _name(n)
{
}

Resource::~Resource()
{
}

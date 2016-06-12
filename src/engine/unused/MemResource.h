#ifndef MEM_RESOURCE_H
#define MEM_RESOURCE_H

#include "Resource.h"

class MemResource : public Resource
{
public:
    MemResource(const char *n, const char *ptr, size_t size)
        : Resource(n, RESOURCE_MEM), _ptr(ptr), _size(size)
    {
    }

    virtual ~MemResource()
    {
        delete [] _ptr;
    }

    inline const void *ptr() const { return _ptr; }
    inline size_t size() const { return _size; }

    virtual unsigned int usedMem() const { return (uint32)_size; }

protected:
    const char *_ptr;
    size_t _size;
};


#endif

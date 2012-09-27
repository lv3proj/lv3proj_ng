#ifndef MEMORY_ALLOCATOR_NULL_H
#define MEMORY_ALLOCATOR_NULL_H

class NullAllocator
{
public:

    NullAllocator() {}

    template <typename DATA>
    NullAllocator(const DATA& md)
    {
    }

    void* Allocate(size_t size, size_t alignment, size_t offset)
    {
        return NULL;
    }

    inline void Free(void* ptr)
    {
    }

    inline size_t GetAllocationSize(const void* ptr)
    {
        return 0;
    }
};

#endif

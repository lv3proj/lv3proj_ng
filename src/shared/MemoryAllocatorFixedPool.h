#ifndef MEMORY_ALLOCATOR_FIXED_POOL_H
#define MEMORY_ALLOCATOR_FIXED_POOL_H

class FixedPoolAllocator
{
public:

    template <typename DATA>
    FixedPoolAllocator(const DATA& md)
        : _memSize(md.size)
        , _elemSize(md.elemSize)
        , _alignment(md.alignment)
        , _offset(md.offset)
    {
        ASSERT(md.ptr);
        ASSERT(md.size && md.elemSize);
        ASSERT(md.size >= md.elemSize);
        ASSERT(md.size >= sizeof(Freelist));

        // Align memory for first bucket
        char *aligned = alignPointer(((char*)md.ptr) + md.offset,md. alignment) - md.offset;
        ptrdiff_t d = aligned - (char*)md.ptr;

        // Construct freelist in memory
        _freelist = new(aligned) Freelist(aligned, aligned + md.size - d, md.elemSize, md.alignment, md.offset);
    }


    void* Allocate(size_t size, size_t alignment, size_t offset)
    {
        ASSERT(size <= _elemSize);
        ASSERT(alignment <= _alignment);
        ASSERT(offset <= _offset);
        return _freelist->Obtain();
    }

    inline void Free(void* ptr)
    {
        ASSERT(ptr >= _freelist && ptr < ((char*)_freelist) + _memSize);
        _freelist->Return(ptr);
    }

    inline size_t GetAllocationSize(const void* ptr)
    {
        return _elemSize;
    }

private:

    Freelist *_freelist;

    size_t _memSize;
    size_t _elemSize;
    size_t _alignment;
    size_t _offset;
};

#endif

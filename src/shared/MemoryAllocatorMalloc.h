#ifndef MEMORY_ALLOCATOR_MALLOC_H
#define MEMORY_ALLOCATOR_MALLOC_H

#include "MemoryUtil.h"


class BasicHeapAllocator
{
public:

    BasicHeapAllocator() {}

    template <typename DATA>
    BasicHeapAllocator(const DATA&)
    {
    }

    // Taken from http://cottonvibes.blogspot.de/2011/01/dynamically-allocate-aligned-memory.html
    void* Allocate(size_t size, size_t alignment, size_t offset)
    {
        size_t allocOffset = offset + sizeof(Header);
        char *mem = (char*)malloc(size + sizeof(Header) + alignment);
        char *aligned = alignPointer(mem + allocOffset, alignment);
        char *pre = aligned - allocOffset;
        char *ret = aligned - offset;
        Header *hdrp = ((Header*)pre);
        hdrp->size = (unsigned int)size;
        hdrp->ptrDiff = (unsigned int)(ret - mem);

        //                     {internal  }{user data .......}
        // layout: [garbage]...[diff][size][offset][aligned]
        //                                          ^-- This address is aligned to <alignment> bytes
        //                                  ^-- this is the returned pointer
        //                            ^-- Size of the requested allocation
        //                      ^-- how many bytes need to be subtracted from passed pointer to free() to get the real pointer
        //                                 [      ] -- <offset> bytes as specified
        //                     [          ] -- additional header space
        //          ^-- this is what malloc() returned
        return ret;
    }

    inline void Free(void* ptr)
    {
        //if (!ptr)
        //    return;
        char *pre = ((char*)ptr) - sizeof(Header);
        char *mem = ((char*)ptr) - ((Header*)pre)->ptrDiff;
        free(mem);
    }

    inline size_t GetAllocationSize(const void* ptr)
    {
        const char *pre = ((const char*)ptr) - sizeof(Header);
        return ((const Header*)pre)->size;
    }

private:
    struct Header
    {
        unsigned int size;
        unsigned int ptrDiff;
    };

};

#endif

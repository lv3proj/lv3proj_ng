#ifndef MEMORY_ALLOCATOR_DYNAMIC_POOL_H
#define MEMORY_ALLOCATOR_DYNAMIC_POOL_H

template <typename PIGGYBACK> class DynamicPoolAllocator
{
public:

    // Ensure the right piggypack allocator is in the template
    template <template <typename> class DATA >
    explicit DynamicPoolAllocator(const DATA<PIGGYBACK>& md)
        : _memSize(md.size)
        , _elemSize(md.elemSize)
        , _offset(md.offset)
        , _alignment(md.alignment)
        , _piggyback(md.piggybackArena)
        , _freelist(NULL)
        , _memstart((char*)md.ptr)
        , _tail(NULL)
    {
        ASSERT(md.elemSize && md.size);
        ASSERT(md.size >= md.elemSize);
        ASSERT(md.size >= sizeof(Freelist) + sizeof(Header));
    }


    void* Allocate(size_t size, size_t alignment, size_t offset)
    {
        ASSERT(offset <= _offset);
        ASSERT(size <= _elemSize);
        ASSERT(alignment <= _alignment);
        if(!_EnsureInitialBlock())
            return NULL;
        char *ptr = _freelist->Obtain();
        if(ptr)
        {
            ++(((Header*)_memstart)->allocatedElems);
            return ptr;
        }

        // Pool full? Make new pool.
        void *nextblock = _piggyback.Allocate(_memSize, _alignment, XSourceInfo(__FILE__, __LINE__));
        if(!nextblock)
            return NULL;

        // When returning blocks to this freelist, they will be appended as usual
        _freelist = _ConstructFreelist(nextblock, _elemSize, _memSize, _alignment, _offset, true);
        ASSERT(!_tail->nexthdr);
        _tail->nexthdr = (Header*)nextblock;
        _tail = (Header*)nextblock;

        ++(((Header*)_memstart)->allocatedElems);
        return _freelist->Obtain();
    }

    inline void Free(void* ptr)
    {
        --(((Header*)_memstart)->allocatedElems);
        _freelist->Return(ptr);
        if(!((Header*)_memstart)->allocatedElems)
            _Cleanup();
    }

    inline size_t GetAllocationSize(const void* ptr)
    {
        return _elemSize;
    }

private:

    inline bool _EnsureInitialBlock()
    {
        if(!_memstart)
        {
            _memstart = (char*)_piggyback.Allocate(_memSize, _alignment, XSourceInfo(__FILE__, __LINE__) );
            if(!_memstart)
                return false;
        }
        if(!_freelist)
        {
            ASSERT(!_tail);
            _freelist = _ConstructFreelist(_memstart, _elemSize, _memSize, _alignment, _offset, true);
            _tail = (Header*)_memstart;
        }
        return true;
    }


    inline Freelist *_ConstructFreelist(void *p, size_t elemSize, size_t size, size_t alignment, size_t offset, bool allocedHere)
    {
        // First address in the block is used as pointer to the next block,
        // with a bit of extra data
        Header *hdr = (Header*)p;
        hdr->nexthdr = NULL;
        hdr->allocatedElems = 0;
        hdr->allocatedHere = allocedHere;

        char *listp = (char*)p + sizeof(Header);
        size_t listsz = size - sizeof(Header);

        // Construct freelist in memory, after the header
        return new(listp) Freelist(listp, listp + listsz, elemSize, alignment, offset);
    }

    inline void _Cleanup()
    {
        Header *hdr = (Header*)_memstart;
        // First block may be static or allocated by piggyback
        if(hdr->allocatedHere)
        {
            _memstart = NULL;
            Header *next = hdr->nexthdr;
            _piggyback.Free(hdr);
            hdr = next;
        }
        else
        {
            // static block
            // nothing else to clean up; _freelist is still where it was and valid, _tail too.
            if(!hdr->nexthdr)
                return;
        }

        // More than one block; so _freelist was changed in the meantime, need to re-init on next allocation.
        // The _tail block will be deleted, too.
        _tail = NULL;
        _freelist = NULL; // memory is gone, so freelist is gone too

        while(hdr)
        {
            Header *next = hdr->nexthdr;
            ASSERT(hdr->allocatedHere);
            _piggyback.Free(hdr);
            hdr = next;
        }
    }

    struct Header
    {
        Header *nexthdr;
        unsigned int allocatedElems; // only used in first header
        unsigned int allocatedHere;
    };

    Freelist *_freelist;
    Header *_tail;

    char *_memstart;

    size_t _memSize;
    size_t _elemSize;
    size_t _alignment;
    size_t _offset;
    PIGGYBACK& _piggyback;
};

#endif

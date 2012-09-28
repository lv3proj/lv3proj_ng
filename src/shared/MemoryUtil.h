#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include <new>

struct XSourceInfo
{
    XSourceInfo() : file("<unk>"), func("<unk>"), line(0) {}
    XSourceInfo(const char *fil, size_t l, const char *fun)
        : file(fil), func(fun), line(l) {}

    const char *file;
    const char *func;
    size_t line;
};


template <class T>
struct TypeAndCount
{
};

template <class T, size_t N>
struct TypeAndCount<T[N]>
{
    typedef T Type;
    static const size_t Count = N;
};

template <typename T>
struct IsPOD
{
    static const bool Value = false;
};

#define XIS_POD(T) (IsPOD<T>::Value)

#define XMEM_DEFINE_POD_TYPE(T) \
    template <> struct IsPOD<T> { static const bool Value = true; }

// 1 byte
XMEM_DEFINE_POD_TYPE(char);
XMEM_DEFINE_POD_TYPE(unsigned char);
// 2 bytes
XMEM_DEFINE_POD_TYPE(short);
XMEM_DEFINE_POD_TYPE(unsigned short);
// 4 bytes
XMEM_DEFINE_POD_TYPE(int);
XMEM_DEFINE_POD_TYPE(unsigned int);
XMEM_DEFINE_POD_TYPE(long);
XMEM_DEFINE_POD_TYPE(unsigned long);
// 8 bytes
XMEM_DEFINE_POD_TYPE(int64);
XMEM_DEFINE_POD_TYPE(uint64);

template <bool I>
struct IntToType
{
};

typedef IntToType<false> NonPODType;
typedef IntToType<true> PODType;


// must be power of 2
inline char *alignPointer(void *ptr, size_t align)
{
    compile_assert(sizeof(intptr_t) == sizeof(void*));
    if(!align)
        return (char*)ptr;
    --align;
    intptr_t p = intptr_t(ptr);
    return (char*)((p + align) & ~align);
}

inline bool pointerIsAligned(void *ptr, size_t align)
{
    return !align || !(intptr_t(ptr) % align);
}

class NullAllocator;

template <typename ARENA = MemoryArena<NullAllocator, SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging> > class MemoryData
{
public:
    MemoryData(void *ptr = NULL, size_t size = 0, size_t elemSize = 0, size_t alignment = 0, size_t offset = 0, ARENA& piggyback = ARENA())
        : ptr(ptr)
        , size(size)
        , elemSize(elemSize)
        , alignment(alignment)
        , offset(offset)
        , piggybackArena(piggyback)
    {
    }

    explicit MemoryData(size_t size, size_t elemSize = 0, size_t alignment = 0, size_t offset = 0, ARENA& piggyback = ARENA())
        : ptr(NULL)
        , size(size)
        , elemSize(elemSize)
        , alignment(alignment)
        , offset(offset)
        , piggybackArena(piggyback)
    {
    }

    void *ptr;
    size_t size;
    size_t elemSize;
    size_t alignment;
    size_t offset;
    ARENA &piggybackArena;
};

#include "MemoryAllocatorNull.h"

class Freelist
{
public:
    Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
    {
        union
        {
            char* as_char;
            Freelist* as_self;
        };

        //ASSERT(pointerIsAligned(start, alignment));

        as_self = (Freelist*)start;

        // initialize the free list - make every m_next of each element point to the next element in the list
        Freelist *runner = as_self;
        const size_t extra = elementSize + offset;
        while(as_char + extra < end)
        {
            runner->m_next = as_self;
            runner = as_self;
            // elementSize already contains offset
            as_char = (char*)alignPointer(as_char + extra, alignment) - offset;
        }

        runner->m_next = NULL;
    }

    inline char* Obtain(void)
    {
        // is there an entry left?
        if (!m_next)
        {
            // we are out of entries
            return NULL;
        }

        // obtain one element from the head of the free list
        Freelist *head = m_next;
        m_next = head->m_next;
        return (char*)head;
    }

    inline void Return(void* ptr)
    {
        // put the returned element at the head of the free list
        Freelist* head = static_cast<Freelist*>(ptr);
        head->m_next = m_next;
        m_next = head;
    }

private:
    Freelist *m_next;
};


#endif

#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include "MemoryUtil.h"
#include "MemoryBoundsCheckPolicy.h"
#include "MemoryTaggingPolicy.h"
#include "MemoryThreadPolicy.h"
#include "MemoryTrackingPolicy.h"
#include "MemoryAllocatorMalloc.h"



template
<
    typename AllocationPolicy,
    typename ThreadPolicy,
    typename BoundsCheckingPolicy,
    typename MemoryTrackingPolicy,
    typename MemoryTaggingPolicy
>
class MemoryArena
{
public:

    MemoryArena()
        //: m_allocator(MemoryData<NullAllocator>())
    {
    }

    /*template <typename DATA>
    explicit MemoryArena(const DATA& md)
        : m_allocator(md)
    {
    }*/

    template <typename PIGGYBACK, template <typename> class DATA>
    explicit MemoryArena(const DATA<PIGGYBACK>& md)
        : m_allocator(DATA<PIGGYBACK>(
            md.ptr,
            md.size,
            md.elemSize + BoundsCheckingPolicy::SIZE_FRONT + BoundsCheckingPolicy::SIZE_BACK,
            md.alignment,
            md.offset + BoundsCheckingPolicy::SIZE_FRONT,
            md.piggybackArena
        ))
    {
    }

    void* Allocate(size_t size, size_t alignment, const XSourceInfo& sourceInfo)
    {
        m_threadGuard.Enter();

        const size_t originalSize = size;
        const size_t newSize = size + BoundsCheckingPolicy::SIZE_FRONT + BoundsCheckingPolicy::SIZE_BACK;

        // Allocate memory so that newSize bytes are allocated, and the returned pointer + offset is aligned
        // (So that offset bytes can be put before the actual pointer given to the user)
        // In short: The pointer returned to the user below is aligned, no matter of the stuff before it.
        char* plainMemory = static_cast<char*>(m_allocator.Allocate(newSize, alignment, BoundsCheckingPolicy::SIZE_FRONT));
        ASSERT(plainMemory);
        char *userPtr = plainMemory + BoundsCheckingPolicy::SIZE_FRONT;
        ASSERT(pointerIsAligned(userPtr, alignment));
        m_boundsChecker.GuardFront(plainMemory);
        m_memoryTagger.TagAllocation(userPtr, originalSize);
        m_boundsChecker.GuardBack(userPtr + originalSize);

        m_memoryTracker.OnAllocation(plainMemory, newSize, alignment, sourceInfo);

        m_threadGuard.Leave();

        return userPtr;
    }

    void Free(void* ptr)
    {
        ASSERT(ptr);

        m_threadGuard.Enter();

        char* originalMemory = static_cast<char*>(ptr) - BoundsCheckingPolicy::SIZE_FRONT;
        // allocationSize already contains BoundsCheckingPolicy extra bytes
        const size_t allocationSize = m_allocator.GetAllocationSize(originalMemory);

        m_boundsChecker.CheckFront(originalMemory);
        m_boundsChecker.CheckBack(originalMemory + (allocationSize - BoundsCheckingPolicy::SIZE_BACK));

        m_memoryTracker.OnDeallocation(originalMemory, allocationSize);

        // This overwrites fence bytes too
        m_memoryTagger.TagDeallocation(originalMemory, allocationSize);

        m_allocator.Free(originalMemory);

        m_threadGuard.Leave();
    }

    inline const MemoryTrackingPolicy& GetTracker() const { return m_memoryTracker; }

private:
    AllocationPolicy m_allocator;
    ThreadPolicy m_threadGuard;
    BoundsCheckingPolicy m_boundsChecker;
    MemoryTrackingPolicy m_memoryTracker;
    MemoryTaggingPolicy m_memoryTagger;
};

#endif

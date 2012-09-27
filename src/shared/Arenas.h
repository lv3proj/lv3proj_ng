#ifndef ARENAS_H
#define ARENAS_H

#include "common.h"
#include "MemoryAllocators.h"

/*
typename AllocationPolicy = BasicHeapAllocator,
typename ThreadPolicy = SingleThreadPolicy,
typename BoundsCheckingPolicy = SimpleBoundsChecking,
typename MemoryTrackingPolicy = NoMemoryTracking,
typename MemoryTaggingPolicy = DeadFillTagging
*/

namespace Arenas
{
    class GlobalArena : public MemoryArena<BasicHeapAllocator>
    {
    public:
        GlobalArena();
    };

    class VectorInterpolation : public MemoryArena<DynamicPoolAllocator<GlobalArena> >
    {
    public:
        VectorInterpolation(GlobalArena&, size_t elements, size_t elemSize);
    };

    class QuadMem : public MemoryArena<DynamicPoolAllocator<GlobalArena> >
    {
    public:
        QuadMem(GlobalArena&, size_t elements, size_t elemSize);
    };

    class EntityMem : public MemoryArena<DynamicPoolAllocator<GlobalArena> >
    {
    public:
        EntityMem(GlobalArena&, size_t elements, size_t elemSize);
    };


    extern GlobalArena fallback;
    extern GlobalArena chunkAlloc;
};

#endif

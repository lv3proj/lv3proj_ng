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

    class VectorInterpolation : public MemoryArena<DynamicPoolAllocator<GlobalArena>, SingleThreadPolicy, SimpleBoundsChecking2, NoMemoryTracking, DeadFillTagging2>
    {
    public:
        VectorInterpolation(GlobalArena&, size_t elements, size_t elemSize);
    };

    /*class VectorInterpolation : public MemoryArena<FixedPoolAllocator>
    {
    public:
        VectorInterpolation(GlobalArena&, size_t elements, size_t elemSize);
    };*/

    extern GlobalArena fallback;
    extern GlobalArena chunkAlloc;
};

#endif

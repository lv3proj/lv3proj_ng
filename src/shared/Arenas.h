#ifndef ARENAS_H
#define ARENAS_H

#include "common.h"
#include "MemoryAllocators.h"


namespace Arenas
{
#ifdef NDEBUG
    typedef SingleThreadPolicy ThreadPolicy;
    typedef NoBoundsChecking BoundsCheckingPolicy;
    typedef NoMemoryTracking MemoryTrackingPolicy;
    typedef NoMemoryTagging MemoryTaggingPolicy;
#else
    typedef SingleThreadPolicy ThreadPolicy;
    typedef SimpleBoundsChecking BoundsCheckingPolicy;
    typedef SimpleMemoryTracking MemoryTrackingPolicy;
    typedef DeadFillTagging MemoryTaggingPolicy;
#endif


    class GlobalArena : public MemoryArena<BasicHeapAllocator,
        SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    {
    public:
        GlobalArena();
    };

    class FallbackArena : public MemoryArena<BasicHeapAllocator,
        ThreadPolicy, BoundsCheckingPolicy, SimpleMemoryTracking, MemoryTaggingPolicy>
    {
    public:
        FallbackArena();
    };

    class ScriptArena : public MemoryArena<BasicHeapAllocator,
        SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>
    {
    public:
        ScriptArena();
    };

    class VectorInterpolation : public MemoryArena<DynamicPoolAllocator<GlobalArena>,
        SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    {
    public:
        VectorInterpolation(GlobalArena&, size_t elements, size_t elemSize);
    };

    class QuadMem : public MemoryArena<DynamicPoolAllocator<GlobalArena>,
        SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    {
    public:
        QuadMem(GlobalArena&, size_t elements, size_t elemSize);
    };

    class EntityMem : public MemoryArena<DynamicPoolAllocator<GlobalArena>,
        SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    {
    public:
        EntityMem(GlobalArena&, size_t elements, size_t elemSize);
    };

    class ObsGridMem : public MemoryArena<DynamicPoolAllocator<GlobalArena>,
        SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    {
    public:
        ObsGridMem(GlobalArena&, size_t elements, size_t elemSize);
    };


    extern FallbackArena fallback;
    extern GlobalArena chunkAlloc;

    // Defined in their individual files
    extern EntityMem entityArena;
    extern VectorInterpolation vectorInterpArena;
    extern QuadMem quadArena;
};

#endif

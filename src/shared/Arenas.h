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


    #define GlobalArenaBase MemoryArena<BasicHeapAllocator, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class GlobalArena : public GlobalArenaBase
    {
    public:
        GlobalArena();
    };

    #define FallbackArenaBase MemoryArena<BasicHeapAllocator, ThreadPolicy, BoundsCheckingPolicy, SimpleMemoryTracking, MemoryTaggingPolicy>
    class FallbackArena : public FallbackArenaBase
    {
    public:
        FallbackArena();
    };

    #define ScriptArenaBase MemoryArena<BasicHeapAllocator, SingleThreadPolicy, NoBoundsChecking, NoMemoryTracking, NoMemoryTagging>
    class ScriptArena : public ScriptArenaBase
    {
    public:
        ScriptArena();
    };

    #define VectorInterpolationBase MemoryArena<DynamicPoolAllocator<GlobalArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class VectorInterpolation : public VectorInterpolationBase
    {
    public:
        VectorInterpolation(GlobalArena&, size_t elements, size_t elemSize);
    };

    #define QuadMemBase MemoryArena<DynamicPoolAllocator<GlobalArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class QuadMem : public QuadMemBase
    {
    public:
        QuadMem(GlobalArena&, size_t elements, size_t elemSize);
    };

    #define EntityMemBase MemoryArena<DynamicPoolAllocator<GlobalArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class EntityMem : public EntityMemBase
    {
    public:
        EntityMem(GlobalArena&, size_t elements, size_t elemSize);
    };

    #define ObsGridMemBase MemoryArena<DynamicPoolAllocator<GlobalArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class ObsGridMem : public ObsGridMemBase
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

#ifndef ARENAS_H
#define ARENAS_H

#include "common.h"
#include "MemorySystem.h"
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


    #define GlobalArenaBase MemoryArena<BasicHeapAllocator, SingleThreadPolicy, BoundsCheckingPolicy, SimpleMemoryTracking, MemoryTaggingPolicy>
    class GlobalArena : public GlobalArenaBase
    {
    public:
        GlobalArena();
    };

    #define ChunkArenaBase MemoryArena<BasicHeapAllocator, SingleThreadPolicy, BoundsCheckingPolicy, SimpleMemoryTracking, MemoryTaggingPolicy>
    class ChunkArena : public ChunkArenaBase
    {
    public:
        ChunkArena();
    };

    #define FallbackArenaBase MemoryArena<BasicHeapAllocator, ThreadPolicy, BoundsCheckingPolicy, SimpleMemoryTracking, MemoryTaggingPolicy>
    class FallbackArena : public FallbackArenaBase
    {
    public:
        FallbackArena();
    };

    #define VectorInterpolationBase MemoryArena<DynamicPoolAllocator<ChunkArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class VectorInterpolation : public VectorInterpolationBase
    {
    public:
        VectorInterpolation(ChunkArena&, size_t elements, size_t elemSize);
    };

    #define QuadMemBase MemoryArena<DynamicPoolAllocator<ChunkArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class QuadMem : public QuadMemBase
    {
    public:
        QuadMem(ChunkArena&, size_t elements, size_t elemSize);
    };

    #define EntityMemBase MemoryArena<DynamicPoolAllocator<ChunkArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class EntityMem : public EntityMemBase
    {
    public:
        EntityMem(ChunkArena&, size_t elements, size_t elemSize);
    };

    #define ObsGridMemBase MemoryArena<DynamicPoolAllocator<ChunkArena>, SingleThreadPolicy, BoundsCheckingPolicy, MemoryTrackingPolicy, MemoryTaggingPolicy>
    class ObsGridMem : public ObsGridMemBase
    {
    public:
        ObsGridMem(ChunkArena&, size_t elements, size_t elemSize);
    };


    extern FallbackArena fallback;
    extern ChunkArena chunkAlloc;

    // Defined in their individual files
    extern EntityMem entityArena;
    extern VectorInterpolation vectorInterpArena;
    extern QuadMem quadArena;
};

#endif

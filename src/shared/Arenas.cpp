#include "Arenas.h"

namespace Arenas {

GlobalArena::GlobalArena()
{
}

ChunkArena::ChunkArena()
{
}

FallbackArena::FallbackArena()
{
}

VectorInterpolation::VectorInterpolation(ChunkArena& underlying, size_t elements, size_t elemSize)
: VectorInterpolationBase(MemoryData<ChunkArena>(elements * elemSize, elemSize, sizeof(void*), 0, underlying))
{
}

QuadMem::QuadMem(ChunkArena& underlying, size_t elements, size_t elemSize)
: QuadMemBase(MemoryData<ChunkArena>(elements * elemSize, elemSize, sizeof(void*), 0, underlying))
{
}

EntityMem::EntityMem(ChunkArena& underlying, size_t elements, size_t elemSize)
: EntityMemBase(MemoryData<ChunkArena>(elements * elemSize, elemSize, sizeof(void*), 0, underlying))
{
}

ObsGridMem::ObsGridMem(ChunkArena& underlying, size_t elements, size_t elemSize)
: ObsGridMemBase(MemoryData<ChunkArena>(elements * elemSize, elemSize, 0, 0, underlying))
{
}


FallbackArena fallback;
ChunkArena chunkAlloc;




} // end namespace Arenas



void* operator new(size_t sz)
{
    return Arenas::fallback.Allocate(sz, sizeof(void*), XMEM_SOURCE_INFO);
}

void* operator new[](size_t sz)
{
    return Arenas::fallback.Allocate(sz, sizeof(void*), XMEM_SOURCE_INFO);
}

void operator delete(void* ptr) throw()
{
    if(ptr)
        Arenas::fallback.Free(ptr);
}

void operator delete[](void* ptr) throw()
{
    if(ptr)
        Arenas::fallback.Free(ptr);
}

void* operator new(size_t sz, const std::nothrow_t&) throw()
{
    return Arenas::fallback.Allocate(sz, sizeof(void*), XMEM_SOURCE_INFO);
}

void* operator new[](size_t sz, const std::nothrow_t&) throw()
{
    return Arenas::fallback.Allocate(sz, sizeof(void*), XMEM_SOURCE_INFO);
}

void operator delete(void* ptr, const std::nothrow_t&) throw()
{
    if(ptr)
        Arenas::fallback.Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) throw()
{
    if(ptr)
        Arenas::fallback.Free(ptr);
}


#include "Arenas.h"

namespace Arenas {


GlobalArena::GlobalArena()
{
}

VectorInterpolation::VectorInterpolation(GlobalArena& underlying, size_t elements, size_t elemSize)
: MemoryArena(MemoryData<GlobalArena>(elements * elemSize, elemSize, 4, 0, underlying))
{
}

QuadMem::QuadMem(GlobalArena& underlying, size_t elements, size_t elemSize)
: MemoryArena(MemoryData<GlobalArena>(elements * elemSize, elemSize, sizeof(void*), 0, underlying))
{
}

EntityMem::EntityMem(GlobalArena& underlying, size_t elements, size_t elemSize)
: MemoryArena(MemoryData<GlobalArena>(elements * elemSize, elemSize, sizeof(void*), 0, underlying))
{
}


GlobalArena fallback;
GlobalArena chunkAlloc;




} // end namespace Arenas

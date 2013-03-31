
set(shared_SRC
Arenas.cpp
Arenas.h
array2d.h
bithacks.h
common.h
IntervalTimer.h
log.cpp
log.h
mathtools.h
MemoryAllocatorDynamicPool.h
MemoryAllocatorFixedPool.h
MemoryAllocatorMalloc.h
MemoryAllocatorNull.h
MemoryAllocators.h
MemoryArena.h
MemoryBoundsCheckPolicy.h
MemorySystem.h
MemoryTaggingPolicy.h
MemoryThreadPolicy.h
MemoryTrackingPolicy.h
MemoryUtil.h
MersenneTwister.h
PlatformSpecific.cpp
PlatformSpecific.h
pugixml.cpp
SysDefs.h
tools.cpp
tools.h
Vector.cpp
Vector.h
)


add_library(shared ${shared_SRC})

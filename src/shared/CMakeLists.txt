
set(shared_SRC
array2d.h
common.h
IntervalTimer.h
log.cpp
log.h
mathtools.h
MersenneTwister.h
PlatformSpecific.cpp
PlatformSpecific.h
pugiconfig.hpp
pugixml.cpp
pugixml.hpp
SysDefs.h
tools.cpp
tools.h
Vector.cpp
Vector.h
)


add_library(shared ${shared_SRC})

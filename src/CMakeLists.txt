
# top include dir for external projects
set(TOP_INCLUDE_DIR
"${CMAKE_CURRENT_SOURCE_DIR}"
"${INCLUDE_ROOT_DIR}"
"${CMAKE_CURRENT_SOURCE_DIR}/zlib"
"${CMAKE_CURRENT_SOURCE_DIR}/png"
"${CMAKE_CURRENT_SOURCE_DIR}/ttvfs"
)

# shared include dir with most of the things required
set(SHARED_INCLUDE_DIR ${TOP_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/shared")

# engine include dir, for game
set(ENGINE_INCLUDE_DIR ${SHARED_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/engine")

# dependencies
add_subdirectory (lua)
add_subdirectory (gme)
add_subdirectory (ttvfs)

# framework
add_subdirectory (shared)

# intermediate
add_subdirectory (engine)

# binaries
add_subdirectory (game)

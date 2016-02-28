set(ui_SRC
ui.h
ui.cpp
window.cpp
imgui_impl_sdl.cpp
imgui_impl_sdl.h
window.h
renderer.h
shader.h
vbo.h
)

find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIRS})

add_subdirectory(opengl)

add_library(ui ${ui_SRC})
target_link_libraries(ui imgui ${SDL2_LIBRARY})


include_directories (${ENGINE_INCLUDE_DIR}) 

add_executable (game
game.rc
LuaConstants.cpp
LuaConstants.h
LuaDefines.h
LuaEngine.cpp
LuaEngine.h
LuaInterface.cpp
LuaInterface.h
main.cpp
ScriptedEngine.cpp
ScriptedEngine.h
ScriptedEntity.cpp
ScriptedEntity.h
)


install(TARGETS game DESTINATION bin)
target_link_libraries(game engine lua shared ${SDL_ALL_LIBS})


find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED Shaders GL SceneGraph Trade Sdl2Application)
find_package(nlohmann_json REQUIRED)


set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

if(TARGET wut)
message("Hello.")
else()
message("Bye!")
endif()


#message(WARNING "${CMAKE_BINARY_DIR}/household/sspacktool/TestSprite/animation_folder/spritey.json + ${CMAKE_BINARY_DIR}/household/sspacktool/TestSprite/animation_folder/spritey.tga")


add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/household/sspacktool/TestSprite/animation_folder/spritey.json ${CMAKE_BINARY_DIR}/household/sspacktool/TestSprite/animation_folder/spritey.tga
${CMAKE_BINARY_DIR}/src/resources_TetrisClone_RESOURCES.cpp
COMMAND wut -y -d ${CMAKE_BINARY_DIR}/household/sspacktool/TestSprite/animation_folder/ -o spritey ${CMAKE_SOURCE_DIR}/household/sspacktool/TestSprite/animation_folder
DEPENDS wut)

if(CMAKE_HOST_WIN32)
set(COPY_COMMAND copy /b)
else()
set(COPY_COMMAND cp)
endif()

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/sprites.tga COMMAND ${COPY_COMMAND} $<SHELL_PATH:${CMAKE_SOURCE_DIR}/src/sprites.tga> $<SHELL_PATH:${CMAKE_BINARY_DIR}/src>)

corrade_add_resource(TetrisClone_RESOURCES ${CMAKE_BINARY_DIR}/resources.conf)

#corrade_add_resource(TexturedTriangle_RESOURCES resources.conf)
#message(WARNING "Hi there ${TetrisClone_RESOURCES}...")
add_executable(TetrisClone  Main.cpp 
drawobjey/Sprite.h 
drawobjey/Sprite.cpp 
drawobjey/SpriteHandler.h
drawobjey/SpriteHandler.cpp
support/Utilities.hpp 
support/PulseTimer.h
support/PulseTimer.cpp
support/Common.h
support/ResourceProfile.h
support/ResourceProfile.cpp
drawobjey/RenderTiles.h 
drawobjey/RenderTiles.cpp 
board/TetrisBoard.h 
board/TetrisBoard.cpp 
tetromino/Tetronimo.h 
tetromino/Tetronimo.cpp 
tetromino/internal/RotGenConstexpr.hpp
TetronimoRendering.h
TetronimoRendering.cpp
TetrominoBlobRender.h
MacroIOMessage.h
MacroIOMessage.cpp
MacroManager.h
MacroManager.cpp
layouti/TetroCompositor.h
layouti/TetroCompositor.cpp
layouti/NodeArrangementUnit.h
layouti/NodeArrangementUnit.cpp
#${CMAKE_BINARY_DIR}/src/resources_TetrisClone_RESOURCES.cpp)
${TetrisClone_RESOURCES})

#message(WARNING ${TexturedTriangle_RESOURCES})


if($<CONFIG:$<CONFIG>>)
message("Nope")
endif()
#add_custom_target(printdebugtypetarg COMMAND ${CMAKE_COMMAND} -E echo "$<CONFIG>")

target_include_directories(TetrisClone PRIVATE ./)
target_compile_features(TetrisClone PRIVATE cxx_std_20)
target_compile_definitions(TetrisClone PRIVATE $<$<CONFIG:Debug>:TETRIS_DEBUG_ASSERTIONS>)
target_link_libraries(TetrisClone PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
	Magnum::Trade
	Magnum::SceneGraph
	Magnum::Shaders
	ctla
	nlohmann_json::nlohmann_json
	)
	#Magnum::MeshTools
	#Magnum::Primitives
	#Magnum::Shaders)
#target_link_libraries(MyApplication IMPORTED ctla)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TetrisClone)

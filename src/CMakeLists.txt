find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED Shaders GL SceneGraph Trade Sdl2Application)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

corrade_add_resource(TexturedTriangle_RESOURCES resources.conf)
add_executable(MyApplication  MyApplication.cpp 
Integrator.h
Integrator.cpp
TexturedTriangleShader.h 
TexturedTriangleShader.cpp 
Sprite.h 
Sprite.cpp 
Utilities.hpp 
RenderTiles.h 
RenderTiles.cpp 
TetrisBoard.h 
TetrisBoard.cpp 
Tetronimo.h 
Tetronimo.cpp 
TetronimoRendering.h
TetronimoRendering.cpp
TetrominoBlobRender.h
MacroIOMessage.h
MacroIOMessage.cpp
${TexturedTriangle_RESOURCES})

target_compile_features(MyApplication PRIVATE cxx_std_17)
target_link_libraries(MyApplication PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
	Magnum::Trade
	Magnum::SceneGraph
	Magnum::Shaders)
	#Magnum::MeshTools
	#Magnum::Primitives
	#Magnum::Shaders)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyApplication)

# CMakeList.txt : CMake project for wut, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_BUILD_TYPE Release)

# Add source to this project's executable.
#find_package(LLVM CONFIG REQUIRED)
#find_package(Clang CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
add_library(cute_headers INTERFACE)
add_library(stb_headers INTERFACE)

target_include_directories(cute_headers INTERFACE ${CMAKE_SOURCE_DIR}/friends)
target_include_directories(stb_headers INTERFACE ${CMAKE_SOURCE_DIR}/friends)

#target_include_directories(libclang INTERFACE ${CLANG_INCLUDE_DIRS})
#target_include_directories(clangIndex INTERFACE ${CLANG_INCLUDE_DIRS})

#target_include_directories(libclang INTERFACE ${CLANG_INCLUDE_DIR})

#find_package(Corrade REQUIRED Containers)
#find_package(Magnum REQUIRED GL Trade MeshTools Primitives)

add_executable (wut "wut.cpp" "wut.h")
target_compile_features(wut PRIVATE cxx_std_17)
target_include_directories(wut PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(wut PRIVATE STBI_NO_JPEG
                                       STBI_NO_PSD
                                       STBI_NO_GIF
                                       STBI_NO_HDR
                                       STBI_NO_PIC
                                       STBI_NO_PNM
                                       )

#target_link_libraries(wut PRIVATE Corrade::Containers Magnum::Magnum Magnum::GL Magnum::MeshTools Magnum::Primitives)
#find_package(Clang CONFIG REQUIRED)
#if(NOT TARGET clangTooling)
#message(WARNING "O.O ----- libclang does not exist")
#endif()

#if(TARGET libclang)
#get_target_property(ME_LEGS libclang IMPORTED_LOCATION_DEBUG)
#get_property(ME_ARMS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#message(WARNING ">.< ---- ${ME_LEGS} &&& ${ME_ARMS}")
#endif()
#target_link_libraries(wut PRIVATE libclang clangIndex)
target_link_libraries(wut PRIVATE nlohmann_json::nlohmann_json CLI11::CLI11 cute_headers stb_headers)
# TODO: Add tests and install targets if needed.

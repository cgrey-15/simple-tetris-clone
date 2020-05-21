cmake_minimum_required(VERSION 3.4)
project(TheTetrisClone)


set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})


#add_subdirectory(friends)
add_subdirectory(household)

find_package(Corrade REQUIRED Main)
configure_file(resources.conf.in resources.conf)
add_subdirectory(src)

#target_link_libraries(MyApplication PRIVATE ctla)

cmake_minimum_required(VERSION 3.26)
project(HierarchicalScenes C)

set(CMAKE_C_STANDARD 23)
add_subdirectory(vendor)

add_executable(HierarchicalScenes main.c)
target_link_libraries(HierarchicalScenes PRIVATE flecs_static raylib)
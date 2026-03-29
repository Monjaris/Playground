cmake_minimum_required(VERSION 3.20)
project(platformer)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

find_package(raylib REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

target_link_libraries(main PRIVATE raylib)

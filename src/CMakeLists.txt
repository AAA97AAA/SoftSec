cmake_minimum_required(VERSION 3.14)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

project(grass C CXX)

find_package(Threads REQUIRED)

set(GRASS_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(io)
# add_subdirectory(regex)
add_subdirectory(core)
add_subdirectory(programs)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -fno-stack-protector -z execstack -std=c++11" CACHE INTERNAL "" FORCE)

add_executable(test main.cpp)
target_link_libraries(test programs core io ${CMAKE_THREAD_LIBS_INIT})
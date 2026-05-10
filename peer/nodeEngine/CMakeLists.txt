cmake_minimum_required(VERSION 3.10)
project(nodeEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(inc)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(nodeEngine ${SOURCES})

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(nodeEngine PRIVATE Threads::Threads)

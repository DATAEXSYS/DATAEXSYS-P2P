cmake_minimum_required(VERSION 3.20) # C++23 features often need newer CMake

project(DATAEXSYS_PEER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# libsodium
# -------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

# -------------------------
# include dirs
# -------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SODIUM_INCLUDE_DIRS}
)

link_directories(${SODIUM_LIBRARY_DIRS})

# -------------------------
# source files
# -------------------------

# Runtime core sources
file(GLOB_RECURSE RUNTIME_SOURCES
    ${CMAKE_SOURCE_DIR}/source/runtime/*.cpp
)

# Main application sources (excluding runtime and examples)
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/source/identity/*.cpp
    ${CMAKE_SOURCE_DIR}/source/main.cpp
)

# -------------------------
# libraries
# -------------------------
add_library(runtime_engine STATIC ${RUNTIME_SOURCES})
target_link_libraries(runtime_engine PUBLIC pthread)

# -------------------------
# executable
# -------------------------
add_executable(peer_node ${APP_SOURCES})
target_link_libraries(peer_node
    runtime_engine
    ${SODIUM_LIBRARIES}
)

# -------------------------
# examples
# -------------------------
add_executable(async_runtime_demo examples/distributed_service.cpp)
target_link_libraries(async_runtime_demo runtime_engine)
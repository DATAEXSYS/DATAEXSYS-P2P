cmake_minimum_required(VERSION 3.16)

project(DATAEXSYS_PEER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/source/*.cpp
)

# -------------------------
# executable
# -------------------------
add_executable(peer_node ${SOURCES})

# -------------------------
# linking
# -------------------------
target_link_libraries(peer_node
    ${SODIUM_LIBRARIES}
    pthread
)
cmake_minimum_required(VERSION 3.20)

project(DATAEXSYS_P2P LANGUAGES C CXX)

# =================================================
# Global language standards
# =================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =================================================
# Global output directories
# =================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

# =================================================
# Global include roots
# =================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/shared/inc
)

# =================================================
# Compiler warnings
# =================================================
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# =================================================
# Build dependency graph
# =================================================

# Core shared layer
add_subdirectory(shared)

# Aggregated backend layer
add_subdirectory(backend)

# Main engine executable
add_subdirectory(nodeEngine)
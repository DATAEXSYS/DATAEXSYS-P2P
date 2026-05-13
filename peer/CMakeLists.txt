cmake_minimum_required(VERSION 3.20)

project(DATAEXSYS_P2P LANGUAGES C CXX)


# -----------------------------
# Global include root (IMPORTANT)
# -----------------------------
include_directories(${CMAKE_SOURCE_DIR}/shared/inc)
# -----------------------------
# Global standards
# -----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -----------------------------
# Output directories
# -----------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

# -----------------------------
# Modules (build graph entry points)
# -----------------------------
add_subdirectory(shared)
add_subdirectory(backend/LocalTrustDiaries)
add_subdirectory(backend/PKCertChain)
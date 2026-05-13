# =============================================================================
# peer/CMakeLists.txt  —  Standalone peer layer entry point
#
# This file exists so individual sub-systems can be configured in isolation
# (e.g. by an IDE opening only the peer/ folder).
#
# The canonical project entry point is the ROOT CMakeLists.txt.
# When built from root, the cmake/backend.cmake, cmake/nodeEngine.cmake, and
# cmake/frontend.cmake modules orchestrate everything directly.
# =============================================================================
cmake_minimum_required(VERSION 3.20)

# Only declare a project when not already inside the root build
if(NOT DEFINED PROJECT_NAME)
    project(peer_standalone
        VERSION     0.1.0
        LANGUAGES   C CXX
    )
    set(CMAKE_CXX_STANDARD          23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS        OFF)
    set(CMAKE_C_STANDARD          11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS        OFF)
endif()

# Route to root cmake modules (works whether called from root or standalone)
set(_PEER_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
include(${_PEER_ROOT}/cmake/shared.cmake)
include(${_PEER_ROOT}/cmake/backend.cmake)
include(${_PEER_ROOT}/cmake/nodeEngine.cmake)

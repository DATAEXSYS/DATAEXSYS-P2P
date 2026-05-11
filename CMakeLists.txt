# =============================================================================
# DATAEXSYS-P2P  ·  Root Build
# Project : peer
# =============================================================================
cmake_minimum_required(VERSION 3.20)

project(peer
    VERSION     0.1.0
    DESCRIPTION "DATAEXSYS Peer-to-Peer Node"
    LANGUAGES   C CXX
)

# ---------------------------------------------------------------------------
# Global language standards  (inherited by every sub-target)
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_C_STANDARD          11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

# ---------------------------------------------------------------------------
# Unified output directories
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------
# cmake module path  →  cmake/ folder at root
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# ---------------------------------------------------------------------------
# Global compile-flag interface target
#   Link any target to PEER::Flags to inherit project-wide diagnostics.
# ---------------------------------------------------------------------------
add_library(peer_compile_flags INTERFACE)
add_library(PEER::Flags ALIAS peer_compile_flags)
target_compile_options(peer_compile_flags INTERFACE
    -Wall -Wextra -Wpedantic
)

# ---------------------------------------------------------------------------
# Feature toggles  (pass -D<option>=ON to cmake)
# ---------------------------------------------------------------------------
option(PEER_BUILD_FRONTEND  "Build the Qt GUI frontend"          OFF)
option(PEER_BUILD_TESTS     "Build all module unit-test suites"  OFF)
option(PEER_BUILD_EXAMPLES  "Build backend example targets"      OFF)

# ---------------------------------------------------------------------------
# Submodule auto-init guard
#   If a required submodule directory is missing or empty, attempt to
#   initialize it automatically via git.  This prevents cryptic
#   "No SOURCES given" errors when cloning without --recurse-submodules.
# ---------------------------------------------------------------------------
set(_REQUIRED_SUBMODULES
    peer/PKCertChain
    peer/RollingSignatures
    peer/LocalTrustDiaries
)
foreach(_sm ${_REQUIRED_SUBMODULES})
    set(_sm_path ${CMAKE_SOURCE_DIR}/${_sm})
    if(NOT EXISTS "${_sm_path}/CMakeLists.txt" AND NOT EXISTS "${_sm_path}/src")
        message(STATUS "[peer] Submodule '${_sm}' not populated — running git submodule update --init")
        execute_process(
            COMMAND git submodule update --init --recursive -- ${_sm}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE _sm_result
        )
        if(NOT _sm_result EQUAL 0)
            message(WARNING
                "Could not auto-init submodule '${_sm}'. "
                "Run: git submodule update --init --recursive")
        endif()
    endif()
endforeach()

# ---------------------------------------------------------------------------
# 1. Backend libraries  (PKCertChain · RollingSignatures · LocalTrustDiaries)
# ---------------------------------------------------------------------------
include(cmake/backend.cmake)

# ---------------------------------------------------------------------------
# 2. Core executable  (nodeEngine)
# ---------------------------------------------------------------------------
include(cmake/nodeEngine.cmake)

# ---------------------------------------------------------------------------
# 3. Qt GUI Frontend  (opt-in)
# ---------------------------------------------------------------------------
if(PEER_BUILD_FRONTEND)
    include(cmake/frontend.cmake)
endif()
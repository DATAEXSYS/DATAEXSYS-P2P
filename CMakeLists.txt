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
# Source directory presence check
#   Confirms the three backend library directories exist before trying
#   to glob their sources.  Catches missing checkouts early with a clear
#   message instead of a cryptic "No SOURCES" cmake error.
# ---------------------------------------------------------------------------
set(_REQUIRED_BACKEND_DIRS
    peer/backend/PKCertChain
    peer/backend/RollingSignatures
    peer/backend/LocalTrustDiaries
)
foreach(_dir ${_REQUIRED_BACKEND_DIRS})
    if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${_dir}")
        message(FATAL_ERROR
            "Required backend directory '${_dir}' not found.\n"
            "If using git submodules, run:\n"
            "  git submodule update --init --recursive")
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
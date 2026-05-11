# =============================================================================
# LocalTrustDiaries — Trust-node diary types and utilities (header-only)
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(LocalTrustDiaries VERSION 1.0.0 LANGUAGES CXX)

# Guard: skip re-configuration when included as a sub-project
if(TARGET LocalTrustDiaries)
    return()
endif()

# ---------------------------------------------------------------------------
# Header collection (informational — shows in IDEs)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE LTD_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/TrustNode/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Types/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h"
)

# ---------------------------------------------------------------------------
# INTERFACE library (header-only — no compiled sources)
# ---------------------------------------------------------------------------
add_library(LocalTrustDiaries INTERFACE)
add_library(DATAEXSYS::LocalTrustDiaries ALIAS LocalTrustDiaries)

target_sources(LocalTrustDiaries INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${LTD_HEADERS}
)

target_include_directories(LocalTrustDiaries
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(LocalTrustDiaries INTERFACE cxx_std_23)

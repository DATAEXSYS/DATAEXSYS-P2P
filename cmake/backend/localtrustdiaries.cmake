# =============================================================================
# cmake/backend/localtrustdiaries.cmake
#
# Defines the LocalTrustDiaries INTERFACE library target (header-only).
# Source lives in the git submodule at:  peer/backend/LocalTrustDiaries/
#
# Exports:
#   PEER::LocalTrustDiaries   — link target for consumers
# =============================================================================
if(TARGET PEER::LocalTrustDiaries)
    return()   # include guard
endif()

# ---------------------------------------------------------------------------
# Header collection  (informational — makes headers visible in IDEs)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _LTD_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/backend/LocalTrustDiaries/TrustNode/*.h"
    "${CMAKE_SOURCE_DIR}/peer/backend/LocalTrustDiaries/Types/*.h"
    "${CMAKE_SOURCE_DIR}/peer/backend/LocalTrustDiaries/utils/*.h"
)

# ---------------------------------------------------------------------------
# Interface (header-only) target
# ---------------------------------------------------------------------------
add_library(LocalTrustDiaries INTERFACE)
add_library(PEER::LocalTrustDiaries ALIAS LocalTrustDiaries)

target_sources(LocalTrustDiaries INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/peer/backend/LocalTrustDiaries
    FILES ${_LTD_HEADERS}
)

target_include_directories(LocalTrustDiaries
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/backend/LocalTrustDiaries>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(LocalTrustDiaries INTERFACE cxx_std_23)

message(STATUS "[backend] LocalTrustDiaries — INTERFACE (header-only) configured")

target_link_libraries(LocalTrustDiaries INTERFACE PEER::Shared)

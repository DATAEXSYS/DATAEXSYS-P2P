# =============================================================================
# cmake/adapters/common.cmake
#
# AdaptersCommon — base interface library shared by all adapters.
# Provides: IAdapter, AdapterMessage, AdapterResult, AdapterOpcode,
#           AdapterErrors, BufferView.
#
# Exports:
#   PEER::AdaptersCommon   — STATIC library
# =============================================================================
if(TARGET PEER::AdaptersCommon)
    return()
endif()

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _AC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/common/src/*.cpp"
)

file(GLOB_RECURSE _AC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/common/include/*.hpp"
)

# ---------------------------------------------------------------------------
# Target
# ---------------------------------------------------------------------------
add_library(AdaptersCommon STATIC ${_AC_SOURCES} ${_AC_HEADERS})
add_library(PEER::AdaptersCommon ALIAS AdaptersCommon)

target_include_directories(AdaptersCommon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/common/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(AdaptersCommon PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(AdaptersCommon
    PUBLIC  PEER::Flags
)

message(STATUS "[adapters] AdaptersCommon — configured")

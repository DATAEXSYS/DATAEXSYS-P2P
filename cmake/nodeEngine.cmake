# =============================================================================
# cmake/nodeEngine.cmake
#
# Core peer execution engine executable.
# Source lives at:  peer/nodeEngine/
#
# Depends on (must be included before this file):
#   cmake/backend.cmake  →  PEER::PKCertChain
#                           PEER::RollingSignatures
#                           PEER::LocalTrustDiaries
#                           Threads::Threads
#
# Produces:
#   nodeEngine   executable  →  bin/nodeEngine
# =============================================================================
if(TARGET nodeEngine)
    return()   # include guard
endif()

# Validate that backend libraries exist before attempting to link
foreach(_dep PEER::PKCertChain PEER::RollingSignatures PEER::LocalTrustDiaries)
    if(NOT TARGET ${_dep})
        message(FATAL_ERROR
            "cmake/nodeEngine.cmake: required target '${_dep}' not found.\n"
            "Make sure cmake/backend.cmake is included before cmake/nodeEngine.cmake.")
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Source collection
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _NE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/nodeEngine/src/*.cpp"
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------
add_executable(nodeEngine ${_NE_SOURCES})

target_include_directories(nodeEngine
    PRIVATE
        ${CMAKE_SOURCE_DIR}/peer/nodeEngine/inc          # #include "classes/..."
        ${CMAKE_SOURCE_DIR}/peer/nodeEngine/inc/classes  # #include "ipv6/ipv6.h" etc.
)

set_target_properties(nodeEngine PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
    OUTPUT_NAME           nodeEngine
)

# ---------------------------------------------------------------------------
# Link dependencies
# ---------------------------------------------------------------------------
target_link_libraries(nodeEngine
    PRIVATE
        Threads::Threads
        PEER::PKCertChain
        PEER::RollingSignatures
        PEER::LocalTrustDiaries
        PEER::Flags
)

message(STATUS "[peer] nodeEngine — executable configured")

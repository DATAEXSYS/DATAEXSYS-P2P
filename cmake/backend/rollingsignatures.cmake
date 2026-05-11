# =============================================================================
# cmake/backend/rollingsignatures.cmake
#
# Defines the RollingSignatures STATIC library target.
# Source lives in the git submodule at:  peer/backend/RollingSignatures/
#
# Exports:
#   PEER::RollingSignatures   — link target for consumers
# =============================================================================
if(TARGET PEER::RollingSignatures)
    return()   # include guard
endif()

# Shared deps must be resolved first
include(${CMAKE_SOURCE_DIR}/cmake/backend/main.cmake)

# ---------------------------------------------------------------------------
# Source collection
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _RS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/backend/RollingSignatures/src/*.c"
)
if(NOT _RS_SOURCES)
    message(FATAL_ERROR
        "RollingSignatures: no source files found in peer/backend/RollingSignatures/src/\n"
        "Submodule may not be checked out. Run:\n"
        "  git submodule update --init --recursive")
endif()

# ---------------------------------------------------------------------------
# Static library target
# ---------------------------------------------------------------------------
add_library(RollingSignatures STATIC ${_RS_SOURCES})
add_library(PEER::RollingSignatures ALIAS RollingSignatures)

target_include_directories(RollingSignatures
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/backend/RollingSignatures/inc>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(RollingSignatures PRIVATE -O2 -Wall -Wextra)

target_link_libraries(RollingSignatures
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)

set_target_properties(RollingSignatures PROPERTIES
    C_STANDARD   11
    C_EXTENSIONS OFF
)

message(STATUS "[backend] RollingSignatures — STATIC library configured")

# ---------------------------------------------------------------------------
# Unit tests  (opt-in via -DPEER_BUILD_TESTS=ON)
# ---------------------------------------------------------------------------
if(PEER_BUILD_TESTS)
    enable_testing()
    if(EXISTS "${CMAKE_SOURCE_DIR}/peer/backend/RollingSignatures/tests/test_hmac_routing.c")
        add_executable(rs_test_hmac_routing
            ${CMAKE_SOURCE_DIR}/peer/backend/RollingSignatures/tests/test_hmac_routing.c
        )
        target_include_directories(rs_test_hmac_routing PRIVATE
            ${CMAKE_SOURCE_DIR}/peer/backend/RollingSignatures/inc
        )
        target_link_libraries(rs_test_hmac_routing PRIVATE
            OpenSSL::SSL OpenSSL::Crypto
        )
        add_test(NAME RollingSignatures::test_hmac_routing
                 COMMAND rs_test_hmac_routing)
    endif()
endif()

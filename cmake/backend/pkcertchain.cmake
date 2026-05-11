# =============================================================================
# cmake/backend/pkcertchain.cmake
#
# Defines the PKCertChain STATIC library target.
# Source lives in the git submodule at:  peer/backend/PKCertChain/
#
# Exports:
#   PEER::PKCertChain   — link target for consumers
# =============================================================================
if(TARGET PEER::PKCertChain)
    return()   # already defined (include guard)
endif()

# Shared deps must be resolved first
include(${CMAKE_SOURCE_DIR}/cmake/backend/main.cmake)

# ---------------------------------------------------------------------------
# Source collection
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _PKC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/backend/PKCertChain/src/*.c"
)
if(NOT _PKC_SOURCES)
    message(FATAL_ERROR
        "PKCertChain: no source files found in peer/backend/PKCertChain/src/\n"
        "Submodule may not be checked out. Run:\n"
        "  git submodule update --init --recursive")
endif()

# ---------------------------------------------------------------------------
# Static library target
# ---------------------------------------------------------------------------
add_library(PKCertChain STATIC ${_PKC_SOURCES})
add_library(PEER::PKCertChain ALIAS PKCertChain)

target_include_directories(PKCertChain
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/backend/PKCertChain/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(PKCertChain PRIVATE _DEFAULT_SOURCE)
target_compile_options(PKCertChain     PRIVATE -O2 -Wall -Wextra)

target_link_libraries(PKCertChain
    PUBLIC OpenSSL::Crypto
)

set_target_properties(PKCertChain PROPERTIES
    C_STANDARD   11
    C_EXTENSIONS OFF
)

message(STATUS "[backend] PKCertChain — STATIC library configured")

# ---------------------------------------------------------------------------
# Unit tests  (opt-in via -DPEER_BUILD_TESTS=ON)
# ---------------------------------------------------------------------------
if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _PKC_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/backend/PKCertChain/tests/*.c"
    )
    foreach(_src ${_PKC_TEST_SOURCES})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(pkc_${_name} ${_src})
        target_include_directories(pkc_${_name} PRIVATE
            ${CMAKE_SOURCE_DIR}/peer/backend/PKCertChain/include
        )
        target_compile_definitions(pkc_${_name} PRIVATE _DEFAULT_SOURCE)
        target_link_libraries(pkc_${_name} PRIVATE OpenSSL::Crypto)
        add_test(NAME PKCertChain::${_name} COMMAND pkc_${_name})
    endforeach()
endif()

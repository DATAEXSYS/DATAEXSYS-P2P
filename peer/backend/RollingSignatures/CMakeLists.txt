# =============================================================================
# RollingSignatures — HMAC-based rolling signature library
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(RollingSignatures VERSION 1.0.0 LANGUAGES C)

# Guard: skip re-configuration when included as a sub-project
if(TARGET RollingSignatures)
    return()
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE RS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------
add_library(RollingSignatures STATIC ${RS_SOURCES})
add_library(DATAEXSYS::RollingSignatures ALIAS RollingSignatures)

target_include_directories(RollingSignatures
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
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

# ---------------------------------------------------------------------------
# Tests (opt-in via DATAEXSYS_BUILD_TESTS or RollingSignatures standalone)
# ---------------------------------------------------------------------------
if(DATAEXSYS_BUILD_TESTS OR (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME))
    enable_testing()

    # Named test known from upstream CMakeLists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_hmac_routing.c")
        add_executable(rs_test_hmac_routing
            tests/test_hmac_routing.c
        )
        target_include_directories(rs_test_hmac_routing PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/inc
        )
        target_link_libraries(rs_test_hmac_routing PRIVATE
            OpenSSL::SSL OpenSSL::Crypto
        )
        add_test(NAME RollingSignatures::test_hmac_routing
                 COMMAND rs_test_hmac_routing)
    endif()
endif()

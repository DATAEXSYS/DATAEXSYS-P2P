# =============================================================================
# PKCertChain — Public-Key Certificate Chain library
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(PKCertChain VERSION 1.0.0 LANGUAGES C)

# Guard: skip re-configuration when included as a sub-project
if(TARGET PKCertChain)
    return()
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE PKCERTCHAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------
add_library(PKCertChain STATIC ${PKCERTCHAIN_SOURCES})
add_library(DATAEXSYS::PKCertChain ALIAS PKCertChain)

target_include_directories(PKCertChain
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(PKCertChain PRIVATE _DEFAULT_SOURCE)

target_compile_options(PKCertChain PRIVATE -O2 -Wall -Wextra)

target_link_libraries(PKCertChain
    PUBLIC  OpenSSL::Crypto
)

set_target_properties(PKCertChain PROPERTIES
    C_STANDARD   11
    C_EXTENSIONS OFF
)

# ---------------------------------------------------------------------------
# Tests (opt-in via DATAEXSYS_BUILD_TESTS or PKCertChain standalone)
# ---------------------------------------------------------------------------
if(DATAEXSYS_BUILD_TESTS OR (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME))
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c"
    )

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(pkc_${test_name} ${test_src})
        target_include_directories(pkc_${test_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
        target_compile_definitions(pkc_${test_name} PRIVATE _DEFAULT_SOURCE)
        target_link_libraries(pkc_${test_name} PRIVATE OpenSSL::Crypto)
        add_test(NAME PKCertChain::${test_name} COMMAND pkc_${test_name})
    endforeach()
endif()

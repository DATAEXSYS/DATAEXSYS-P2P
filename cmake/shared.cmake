# =============================================================================
# cmake/shared.cmake
#
# Shared module orchestrator — defines the PEER::Shared header-only library
# containing all common data structures, enums, and utilities.
# =============================================================================

add_library(peer_shared INTERFACE)
add_library(PEER::Shared ALIAS peer_shared)

target_include_directories(peer_shared INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/shared/inc>
)

# OpenSSL is required by some crypto utils in shared/inc
find_package(OpenSSL REQUIRED)
target_link_libraries(peer_shared INTERFACE OpenSSL::Crypto)

message(STATUS "[peer] Shared library (Data Structures & Utils) loaded")

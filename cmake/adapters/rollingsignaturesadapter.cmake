# =============================================================================
# cmake/adapters/rollingsignaturesadapter.cmake
#
# RollingSignaturesAdapter — sole ABI bridge to the RollingSignatures C library.
# Exports: PEER::RollingSignaturesAdapter
# =============================================================================
if(TARGET PEER::RollingSignaturesAdapter)
    return()
endif()

file(GLOB_RECURSE _RSA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/RollingSignaturesAdapter/src/*.cpp"
)

add_library(RollingSignaturesAdapter STATIC ${_RSA_SOURCES})
add_library(PEER::RollingSignaturesAdapter ALIAS RollingSignaturesAdapter)

target_include_directories(RollingSignaturesAdapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/RollingSignaturesAdapter/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(RollingSignaturesAdapter PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(RollingSignaturesAdapter
    PUBLIC
        PEER::AdaptersCommon
        PEER::Flags
    PRIVATE
        PEER::RollingSignatures  # C backend — kept PRIVATE, not leaked to consumers
)

if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _RSA_TESTS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/adapters/RollingSignaturesAdapter/tests/*.cpp"
    )
    foreach(_src ${_RSA_TESTS})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(rsa_${_name} ${_src})
        target_link_libraries(rsa_${_name} PRIVATE PEER::RollingSignaturesAdapter)
        add_test(NAME RollingSignaturesAdapter::${_name} COMMAND rsa_${_name})
    endforeach()
endif()

message(STATUS "[adapters] RollingSignaturesAdapter — configured")

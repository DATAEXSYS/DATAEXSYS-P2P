# =============================================================================
# cmake/adapters/networkadapter.cmake
#
# NetworkAdapter — packet ingress/egress boundary.
# Exports: PEER::NetworkAdapter
# =============================================================================
if(TARGET PEER::NetworkAdapter)
    return()
endif()

file(GLOB_RECURSE _NA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/NetworkAdapter/src/*.cpp"
)

add_library(NetworkAdapter STATIC ${_NA_SOURCES})
add_library(PEER::NetworkAdapter ALIAS NetworkAdapter)

target_include_directories(NetworkAdapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/NetworkAdapter/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(NetworkAdapter PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(NetworkAdapter
    PUBLIC
        PEER::AdaptersCommon
        PEER::Flags
)

if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _NA_TESTS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/adapters/NetworkAdapter/tests/*.cpp"
    )
    foreach(_src ${_NA_TESTS})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(na_${_name} ${_src})
        target_link_libraries(na_${_name} PRIVATE PEER::NetworkAdapter)
        add_test(NAME NetworkAdapter::${_name} COMMAND na_${_name})
    endforeach()
endif()

message(STATUS "[adapters] NetworkAdapter — configured")

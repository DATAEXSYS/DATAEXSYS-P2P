# =============================================================================
# cmake/adapters/frontendadapter.cmake
#
# FrontendAdapter — IPC bridge between nodeEngine and the Qt frontend.
# Provides JSON serialisation hooks, event broadcasting, status propagation.
# Does NOT link Qt directly — the bridge uses plain C++23 and AdapterMessage.
#
# Exports: PEER::FrontendAdapter
# =============================================================================
if(TARGET PEER::FrontendAdapter)
    return()
endif()

file(GLOB_RECURSE _FA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/FrontendAdapter/src/*.cpp"
)

add_library(FrontendAdapter STATIC ${_FA_SOURCES})
add_library(PEER::FrontendAdapter ALIAS FrontendAdapter)

target_include_directories(FrontendAdapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/FrontendAdapter/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(FrontendAdapter PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(FrontendAdapter
    PUBLIC
        PEER::AdaptersCommon
        PEER::Flags
    # Qt is NOT linked here. The frontend links FrontendAdapter and adds Qt itself.
    # This keeps FrontendAdapter buildable without Qt present.
)

if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _FA_TESTS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/adapters/FrontendAdapter/tests/*.cpp"
    )
    foreach(_src ${_FA_TESTS})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(fa_${_name} ${_src})
        target_link_libraries(fa_${_name} PRIVATE PEER::FrontendAdapter)
        add_test(NAME FrontendAdapter::${_name} COMMAND fa_${_name})
    endforeach()
endif()

message(STATUS "[adapters] FrontendAdapter — configured")

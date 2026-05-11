# =============================================================================
# cmake/adapters/localtrustdiariesadapter.cmake
#
# LocalTrustDiariesAdapter — sole bridge to the LocalTrustDiaries header-only lib.
# Exports: PEER::LocalTrustDiariesAdapter
# =============================================================================
if(TARGET PEER::LocalTrustDiariesAdapter)
    return()
endif()

file(GLOB_RECURSE _LTDA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/LocalTrustDiariesAdapter/src/*.cpp"
)

add_library(LocalTrustDiariesAdapter STATIC ${_LTDA_SOURCES})
add_library(PEER::LocalTrustDiariesAdapter ALIAS LocalTrustDiariesAdapter)

target_include_directories(LocalTrustDiariesAdapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/LocalTrustDiariesAdapter/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(LocalTrustDiariesAdapter PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(LocalTrustDiariesAdapter
    PUBLIC
        PEER::AdaptersCommon
        PEER::Flags
    PRIVATE
        PEER::LocalTrustDiaries  # header-only — PRIVATE: TrustNode must NOT leak
)

if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _LTDA_TESTS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/adapters/LocalTrustDiariesAdapter/tests/*.cpp"
    )
    foreach(_src ${_LTDA_TESTS})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(ltda_${_name} ${_src})
        target_link_libraries(ltda_${_name} PRIVATE PEER::LocalTrustDiariesAdapter)
        add_test(NAME LocalTrustDiariesAdapter::${_name} COMMAND ltda_${_name})
    endforeach()
endif()

message(STATUS "[adapters] LocalTrustDiariesAdapter — configured")

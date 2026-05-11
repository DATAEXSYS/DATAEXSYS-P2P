# =============================================================================
# cmake/adapters/pkcertchainadapter.cmake
#
# PKCertChainAdapter — sole ABI bridge to the PKCertChain C library.
# Exports: PEER::PKCertChainAdapter
#
# IMPORTANT: This is the ONLY cmake target permitted to link PEER::PKCertChain
#            for the purpose of C ↔ C++ bridging.  No other adapter or
#            runtime target may link PKCertChain directly.
# =============================================================================
if(TARGET PEER::PKCertChainAdapter)
    return()
endif()

file(GLOB_RECURSE _PKCA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/adapters/PKCertChainAdapter/src/*.cpp"
)

add_library(PKCertChainAdapter STATIC ${_PKCA_SOURCES})
add_library(PEER::PKCertChainAdapter ALIAS PKCertChainAdapter)

target_include_directories(PKCertChainAdapter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/peer/adapters/PKCertChainAdapter/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(PKCertChainAdapter PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(PKCertChainAdapter
    PUBLIC
        PEER::AdaptersCommon
        PEER::Flags
    PRIVATE
        PEER::PKCertChain        # C backend — kept PRIVATE, not leaked to consumers
)

if(PEER_BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE _PKCA_TESTS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/peer/adapters/PKCertChainAdapter/tests/*.cpp"
    )
    foreach(_src ${_PKCA_TESTS})
        get_filename_component(_name ${_src} NAME_WE)
        add_executable(pkca_${_name} ${_src})
        target_link_libraries(pkca_${_name} PRIVATE PEER::PKCertChainAdapter)
        add_test(NAME PKCertChainAdapter::${_name} COMMAND pkca_${_name})
    endforeach()
endif()

message(STATUS "[adapters] PKCertChainAdapter — configured")

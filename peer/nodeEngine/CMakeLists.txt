# =============================================================================
# nodeEngine — Core peer execution engine (executable)
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(nodeEngine VERSION 0.1.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB_RECURSE NE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------
add_executable(nodeEngine ${NE_SOURCES})

target_include_directories(nodeEngine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc          # for #include "classes/..."
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/classes  # for #include "ipv6/ipv6.h" etc.
)

# ---------------------------------------------------------------------------
# C++23 standard
# ---------------------------------------------------------------------------
set_target_properties(nodeEngine PROPERTIES
    CXX_STANDARD          23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(Threads REQUIRED)

target_link_libraries(nodeEngine
    PRIVATE
        Threads::Threads
        DATAEXSYS::PKCertChain
        DATAEXSYS::RollingSignatures
        DATAEXSYS::LocalTrustDiaries
)

# ---------------------------------------------------------------------------
# Compile flags (inherit project-wide flags if defined)
# ---------------------------------------------------------------------------
if(TARGET DATAEXSYS::CompileFlags)
    target_link_libraries(nodeEngine PRIVATE DATAEXSYS::CompileFlags)
endif()

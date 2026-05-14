cmake_minimum_required(VERSION 3.20)

project(nodeEngine LANGUAGES C CXX)

# -------------------------------------------------
# Collect all source files
# -------------------------------------------------
file(GLOB_RECURSE NODEENGINE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

# -------------------------------------------------
# Main executable
# -------------------------------------------------
add_executable(nodeEngine
    ${NODEENGINE_SOURCES}
)

# -------------------------------------------------
# Include directories
# -------------------------------------------------
target_include_directories(nodeEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# -------------------------------------------------
# Link consolidated backend layer
# backend already propagates:
#   - LocalTrustDiaries
#   - PKCertChain
#   - RollingSignatures
#   - shared
# -------------------------------------------------
target_link_libraries(nodeEngine PUBLIC
    backend
)

# -------------------------------------------------
# Language standards
# -------------------------------------------------
target_compile_features(nodeEngine PUBLIC
    cxx_std_23
)

set_target_properties(nodeEngine PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF

    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# -------------------------------------------------
# Compiler warnings
# -------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nodeEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# -------------------------------------------------
# Output directory
# Final binary:
# peer/build/nodeEngine/nodeEngine
# -------------------------------------------------
set_target_properties(nodeEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/nodeEngine
)
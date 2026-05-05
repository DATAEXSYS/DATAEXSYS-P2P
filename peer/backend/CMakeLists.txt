cmake_minimum_required(VERSION 3.20) # C++23 features often need newer CMake

project(DATAEXSYS_PEER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# libsodium
# -------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

# -------------------------
# include dirs
# -------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SODIUM_INCLUDE_DIRS}
)

link_directories(${SODIUM_LIBRARY_DIRS})
# source files
# -------------------------

# Runtime core sources
set(RUNTIME_SOURCES
    source/runtime/scheduler.cpp
    source/runtime/worker.cpp
    source/task/task_queue.cpp
)

# Main application sources
set(APP_SOURCES
    source/main.cpp
    source/identity/identity_service.cpp
)

# -------------------------
# libraries
# -------------------------
add_library(runtime_engine STATIC ${RUNTIME_SOURCES})
target_compile_features(runtime_engine PUBLIC cxx_std_23)
target_include_directories(runtime_engine PUBLIC include ${SODIUM_INCLUDE_DIRS})
target_link_libraries(runtime_engine PUBLIC pthread)
target_compile_options(runtime_engine PRIVATE -Wall -Wextra -Wpedantic)

# -------------------------
# executable
# -------------------------
add_executable(peer_node ${APP_SOURCES})
target_link_libraries(peer_node
    runtime_engine
    ${SODIUM_LIBRARIES}
)

# -------------------------
# examples
# -------------------------
add_executable(async_runtime_demo examples/distributed_service.cpp)
target_link_libraries(async_runtime_demo runtime_engine)
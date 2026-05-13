cmake_minimum_required(VERSION 3.20)

# -----------------------------
# Shared module (header-only core layer)
# -----------------------------
project(shared LANGUAGES C CXX)

# -----------------------------
# Standards (applied to consumers)
# -----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -----------------------------
# Create INTERFACE library (IMPORTANT)
# -----------------------------
add_library(shared INTERFACE)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# -----------------------------
# Compiler requirements (propagated)
# -----------------------------
target_compile_features(shared INTERFACE cxx_std_23)

target_compile_options(shared INTERFACE
    -Wall
    -Wextra
    -Wshadow
    -Wconversion
)

# -----------------------------
# OpenSSL dependency (used by headers like EncUtils / SignUtils)
# -----------------------------
find_package(OpenSSL REQUIRED)

target_link_libraries(shared INTERFACE
    OpenSSL::Crypto
    OpenSSL::SSL
)
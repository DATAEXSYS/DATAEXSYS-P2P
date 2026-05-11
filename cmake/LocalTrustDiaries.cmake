# =========================
# LocalTrustDiaries Module
# =========================

set(LTD_DIR ${CMAKE_SOURCE_DIR}/peer/LocalTrustDiaries)

file(GLOB_RECURSE LTD_SOURCES
    ${LTD_DIR}/TrustNode/*.cpp
    ${LTD_DIR}/Types/*.cpp
    ${LTD_DIR}/utils/*.cpp
)

file(GLOB_RECURSE LTD_HEADERS
    ${LTD_DIR}/TrustNode/*.h
    ${LTD_DIR}/Types/*.h
    ${LTD_DIR}/utils/*.h
)

add_library(LocalTrustDiaries STATIC
    ${LTD_SOURCES}
    ${LTD_HEADERS}
)

target_include_directories(LocalTrustDiaries PUBLIC
    ${LTD_DIR}
)

target_compile_features(LocalTrustDiaries PUBLIC cxx_std_23)

target_compile_options(LocalTrustDiaries PRIVATE
    -Wall -Wextra -Wpedantic
)
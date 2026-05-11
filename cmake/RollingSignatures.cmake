set(RS_DIR ${CMAKE_SOURCE_DIR}/peer/RollingSignatures)

file(GLOB RS_SOURCES ${RS_DIR}/src/*.c)

add_library(RollingSignatures STATIC ${RS_SOURCES})

target_include_directories(RollingSignatures PUBLIC
    ${RS_DIR}/inc
)

find_package(OpenSSL REQUIRED)

target_link_libraries(RollingSignatures PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
)
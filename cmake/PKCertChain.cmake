set(PKC_DIR ${CMAKE_SOURCE_DIR}/peer/PKCertChain)

file(GLOB_RECURSE PKC_SOURCES ${PKC_DIR}/src/*.c)

add_library(PKCertChain STATIC ${PKC_SOURCES})

target_include_directories(PKCertChain PUBLIC
    ${PKC_DIR}/include
)

find_package(OpenSSL REQUIRED)

target_link_libraries(PKCertChain PUBLIC OpenSSL::Crypto)
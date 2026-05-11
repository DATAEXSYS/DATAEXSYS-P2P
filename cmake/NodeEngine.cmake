set(NE_DIR ${CMAKE_SOURCE_DIR}/peer/nodeEngine)

file(GLOB_RECURSE NE_SOURCES ${NE_DIR}/src/*.cpp)

add_executable(nodeEngine ${NE_SOURCES})

target_include_directories(nodeEngine PRIVATE
    ${NE_DIR}/inc
)

find_package(Threads REQUIRED)

target_link_libraries(nodeEngine PRIVATE
    Threads::Threads
    PKCertChain
    RollingSignatures
    LocalTrustDiaries
)
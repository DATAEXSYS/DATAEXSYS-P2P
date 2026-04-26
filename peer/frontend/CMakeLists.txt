cmake_minimum_required(VERSION 3.5)

project(P2PFrontend)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

include_directories(include)

file(GLOB SOURCES
    src/*.cpp
    include/*.h
)

add_executable(P2PFrontend ${SOURCES})

target_link_libraries(P2PFrontend Qt5::Widgets Qt5::Network)
cmake_minimum_required(VERSION 3.5)

project(P2PFrontend)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network Quick Qml)

# =========================
# INCLUDE PATHS
# =========================
include_directories(include src)

# =========================
# RECURSIVE SOURCE COLLECTION
# =========================

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

file(GLOB_RECURSE HEADERS
    include/*.h
)

# =========================
# EXECUTABLE
# =========================

add_executable(P2PFrontend
    ${SOURCES}
    ${HEADERS}
    qml.qrc
)

target_link_libraries(P2PFrontend
    Qt5::Widgets
    Qt5::Network
    Qt5::Quick
    Qt5::Qml
)
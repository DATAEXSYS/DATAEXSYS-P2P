# =============================================================================
# frontend — DATAEXSYS P2P Qt6 GUI frontend
# =============================================================================
cmake_minimum_required(VERSION 3.20)
project(P2PFrontend VERSION 0.1.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Standards & Qt automation
# ---------------------------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
) 2>/dev/null || true  # target not yet defined; set globally below

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Qt6 (preferred) with Qt5 fallback
# ---------------------------------------------------------------------------
find_package(Qt6 QUIET COMPONENTS Widgets Network Quick Qml)
if(Qt6_FOUND)
    set(QT_TARGET Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Network Quick Qml)
    set(QT_TARGET Qt5)
    message(STATUS "Qt6 not found, falling back to Qt5")
endif()

# ---------------------------------------------------------------------------
# Sources & headers
# ---------------------------------------------------------------------------
file(GLOB_RECURSE FRONTEND_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE FRONTEND_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(P2PFrontend
    ${FRONTEND_SOURCES}
    ${FRONTEND_HEADERS}
    qml.qrc
)

target_include_directories(P2PFrontend
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(P2PFrontend
    PRIVATE
        ${QT_TARGET}::Widgets
        ${QT_TARGET}::Network
        ${QT_TARGET}::Quick
        ${QT_TARGET}::Qml
)

if(TARGET DATAEXSYS::CompileFlags)
    target_link_libraries(P2PFrontend PRIVATE DATAEXSYS::CompileFlags)
endif()
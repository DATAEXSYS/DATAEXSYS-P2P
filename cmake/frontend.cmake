# =============================================================================
# cmake/frontend.cmake
#
# Qt GUI frontend for the DATAEXSYS P2P peer node.
# Source lives at:  peer/frontend/
#
# Enable with:  cmake -DPEER_BUILD_FRONTEND=ON
#
# Produces:
#   P2PFrontend   executable  →  bin/P2PFrontend
# =============================================================================
if(TARGET P2PFrontend)
    return()   # include guard
endif()

# ---------------------------------------------------------------------------
# Qt automation
# ---------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Qt6 (preferred) → Qt5 fallback
# ---------------------------------------------------------------------------
find_package(Qt6 QUIET COMPONENTS Widgets Network Quick Qml)
if(Qt6_FOUND)
    set(_QT Qt6)
    message(STATUS "[peer] Frontend — using Qt6 ${Qt6_VERSION}")
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Network Quick Qml)
    set(_QT Qt5)
    message(STATUS "[peer] Frontend — Qt6 not found, using Qt5 ${Qt5_VERSION}")
endif()

# ---------------------------------------------------------------------------
# Source collection
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _FE_SOURCES  CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/frontend/src/*.cpp"
)
file(GLOB_RECURSE _FE_HEADERS  CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/peer/frontend/include/*.h"
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------
add_executable(P2PFrontend
    ${_FE_SOURCES}
    ${_FE_HEADERS}
    ${CMAKE_SOURCE_DIR}/peer/frontend/qml.qrc
)

target_include_directories(P2PFrontend
    PRIVATE
        ${CMAKE_SOURCE_DIR}/peer/frontend/include
        ${CMAKE_SOURCE_DIR}/peer/frontend/src
)

set_target_properties(P2PFrontend PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS        OFF
)

target_link_libraries(P2PFrontend
    PRIVATE
        ${_QT}::Widgets
        ${_QT}::Network
        ${_QT}::Quick
        ${_QT}::Qml
        PEER::Flags
)

message(STATUS "[peer] P2PFrontend — executable configured (${_QT})")

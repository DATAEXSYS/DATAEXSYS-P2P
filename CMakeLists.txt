cmake_minimum_required(VERSION 3.20)

project(DATAEXSYS_P2P LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)

include(cmake/PKCertChain.cmake)
include(cmake/RollingSignatures.cmake)
include(cmake/LocalTrustDiaries.cmake)
include(cmake/NodeEngine.cmake)
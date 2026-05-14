cmake_minimum_required(VERSION 3.20)

project(backend LANGUAGES C CXX)

# -------------------------------------------------
# Build backend modules
# -------------------------------------------------
add_subdirectory(LocalTrustDiaries)
add_subdirectory(PKCertChain)
add_subdirectory(RollingSignatures)

# -------------------------------------------------
# Aggregate backend interface library
# -------------------------------------------------
add_library(backend INTERFACE)

# -------------------------------------------------
# Link all backend systems
# -------------------------------------------------
target_link_libraries(backend INTERFACE
    LocalTrustDiaries
    PKCertChain
    RollingSignatures
)

# -------------------------------------------------
# Shared dependency propagation
# -------------------------------------------------
target_link_libraries(backend INTERFACE
    shared
)
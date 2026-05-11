# =============================================================================
# cmake/backend.cmake
#
# Backend orchestrator — pulls in all C/C++ peer libraries in dependency order.
#
# Sub-modules included:
#   cmake/backend/main.cmake             ← shared dep discovery (OpenSSL, Threads)
#   cmake/backend/pkcertchain.cmake      ← PEER::PKCertChain
#   cmake/backend/rollingsignatures.cmake← PEER::RollingSignatures
#   cmake/backend/localtrustdiaries.cmake← PEER::LocalTrustDiaries
#
# All targets are namespaced under PEER:: and can be linked by any
# sub-system in the project.
# =============================================================================

# 1. Shared dependency discovery (OpenSSL, Threads)
include(${CMAKE_SOURCE_DIR}/cmake/backend/main.cmake)

# 2. Public-Key Certificate Chain  (C, OpenSSL::Crypto)
include(${CMAKE_SOURCE_DIR}/cmake/backend/pkcertchain.cmake)

# 3. HMAC Rolling Signatures       (C, OpenSSL::SSL + Crypto)
include(${CMAKE_SOURCE_DIR}/cmake/backend/rollingsignatures.cmake)

# 4. Local Trust Diaries           (C++23, header-only)
include(${CMAKE_SOURCE_DIR}/cmake/backend/localtrustdiaries.cmake)

message(STATUS "[peer] Backend libraries loaded")

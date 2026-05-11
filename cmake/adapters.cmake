# =============================================================================
# cmake/adapters.cmake
#
# Adapter Layer orchestrator.
# Includes all adapter sub-cmake files in dependency order.
#
# Dependency chain:
#   AdaptersCommon (base interfaces)
#     └─► NetworkAdapter
#     └─► PKCertChainAdapter        (→ PEER::PKCertChain)
#     └─► RollingSignaturesAdapter  (→ PEER::RollingSignatures)
#     └─► LocalTrustDiariesAdapter  (→ PEER::LocalTrustDiaries)
#     └─► FrontendAdapter
#
# All targets are namespaced under PEER::
# =============================================================================

# 1. Common base interfaces (IAdapter, AdapterMessage, AdapterResult, ...)
include(${CMAKE_SOURCE_DIR}/cmake/adapters/common.cmake)

# 2. Individual adapters
include(${CMAKE_SOURCE_DIR}/cmake/adapters/networkadapter.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/adapters/pkcertchainadapter.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/adapters/rollingsignaturesadapter.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/adapters/localtrustdiariesadapter.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/adapters/frontendadapter.cmake)

message(STATUS "[peer] Adapter layer loaded")

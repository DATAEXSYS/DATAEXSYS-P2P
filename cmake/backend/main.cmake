# =============================================================================
# cmake/backend/main.cmake
#
# Shared dependency discovery for all backend libraries.
# Finds each package exactly once regardless of how many sub-modules
# include this file (guard prevents duplicate find_package calls).
# =============================================================================
if(DEFINED _PEER_BACKEND_MAIN_INCLUDED)
    return()
endif()
set(_PEER_BACKEND_MAIN_INCLUDED TRUE)

# ---------------------------------------------------------------------------
# OpenSSL  — used by PKCertChain and RollingSignatures
# ---------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)
message(STATUS "[backend] OpenSSL ${OPENSSL_VERSION} → ${OPENSSL_INCLUDE_DIR}")

# ---------------------------------------------------------------------------
# Threads — used by the runtime engine inside backend
# ---------------------------------------------------------------------------
find_package(Threads REQUIRED)
message(STATUS "[backend] Threads found")

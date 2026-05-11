# DATAEXSYS-P2P — System Architecture

> **Decentralized Authenticated Transfer and Access Exchange SYStem**
> Linux-native, IPv6-mesh, cryptographically authenticated peer-to-peer network node.

---

## 1. Top-Level System Overview

```
DATAEXSYS-P2P/
├── Bootstrapper/          Java 21 / Spring Boot — global peer registry
├── peer/
│   ├── backend/
│   │   ├── PKCertChain/         Identity & consensus layer (C11, OpenSSL)
│   │   ├── RollingSignatures/   Transport authentication layer (C11, OpenSSL)
│   │   └── LocalTrustDiaries/   Trust-score ledger (C++23, header-only)
│   ├── nodeEngine/              Core task-execution runtime (C++23)
│   └── frontend/                Qt6 QML control-plane dashboard (C++17)
└── cmake/
    ├── backend.cmake            Orchestrates all backend libs
    ├── nodeEngine.cmake         nodeEngine executable
    ├── frontend.cmake           Qt frontend (opt-in)
    └── backend/
        ├── main.cmake           Shared dep discovery (OpenSSL, Threads)
        ├── pkcertchain.cmake
        ├── rollingsignatures.cmake
        └── localtrustdiaries.cmake
```

---

## 2. Layer Separation

### Layer 1 — Global Node (Bootstrapper)

| Property | Detail |
|---|---|
| Role | Centralized / federated initial-peer registry |
| Tech | Java 21, Spring Boot 3.4, MySQL 8, Flyway |
| API | RESTful; peer registration, active counts, bootstrap URLs |
| Deployment | Docker-containerised (`Bootstrapper/Dockerfile`) |

Solves the cold-start problem: a new peer has nowhere to discover others without a well-known registry endpoint. Once connected to the mesh, this dependency drops away.

---

### Layer 2 — Peer Backend (`peer/backend/`)

Three independently versioned, git-submodule C libraries. Each is built as a **STATIC** or **INTERFACE** library and linked into `nodeEngine`.

#### 2a. PKCertChain (`peer/backend/PKCertChain/`)

**Purpose:** Public-key identity management via a blockchain. Each node is identified by an Ed25519 keypair; its certificate and full proof-of-work history are stored on a per-node chain.

**Dual-layer internal design:**

```
┌──────────────────────────────────────────────────────┐
│  User-Space Layer  (BlockchainInterface)              │
│  • Blockchain logic, ledger, consensus, fork-resolve  │
│  • Converts kernel buffers → structured blocks        │
│  • Applies PoW verification + signature checks        │
├──────────────────────────────────────────────────────┤
│  Kernel Layer  (KernelBlockInteractor)  [planned]     │
│  • Packet reception, streaming, event dispatch        │
│  • PoW capability classification                      │
│  • Zero-copy buffer handoff to user-space             │
└──────────────────────────────────────────────────────┘
```

**Consensus — two-phase PoW:**

```
verify_prev_block
      │
      ▼
give_mini_pow_challenge  ──►  MiniPoW solve  ──►  Tier classification
                                                         │
                                                         ▼
                                              give_tier_pow_challenge
                                                         │
                                                         ▼
                                              verify_tier_pow_solution
                                                         │
                                                         ▼
                                                    add_block
```

**Tiers (node capability classes):**

| Tier | Target Hardware |
|---|---|
| MCU | Microcontrollers / embedded |
| Edge | Raspberry Pi / edge compute |
| Desktop | Standard workstations |
| Server | High-core-count server nodes |

Difficulty updates are Bayesian, clamped to `[1, 255]`, targeting a 600-second solve window per tier.

**Canonical serialization rule:** Every integer is serialized in **network byte order (big-endian)** before hashing or signing. Native structs are never hashed directly.

---

#### 2b. RollingSignatures (`peer/backend/RollingSignatures/`)

**Purpose:** Packet-level authentication across multi-hop routes using a rolling HMAC chain. Prevents replay attacks and path-tampering without expensive asymmetric crypto at every hop.

**Packet layout:**
```
[ Payload ] ‖ [ Path Vector ] ‖ [ HMAC (32 bytes) ] ‖ [ Nonce ]
```

**Key derivation cascade:**
```
K₀  (master secret, shared by source & destination)
K₁ = SHA256( K₀ ‖ NodeID₁ )
K₂ = SHA256( K₁ ‖ NodeID₂ )
...
Kₙ = SHA256( Kₙ₋₁ ‖ NodeIDₙ )
```

**Three protocol states:**

| State | Actor | Action |
|---|---|---|
| `create_packet` | Source | Initialises packet with `K₀` and nonce |
| `forward_packet` | Intermediate hop | Verify ← Append NodeID ← Derive Kᵢ ← Recompute HMAC |
| `verify_packet` | Destination | Replays key chain, confirms final HMAC |

If any hop tampers with payload or path vector, the key-chain collapse causes destination rejection.

---

#### 2c. LocalTrustDiaries (`peer/backend/LocalTrustDiaries/`)

**Purpose:** Per-node trust ledger. Tracks interaction history (ACKs / NACKs) and a live trust score for each known peer. Header-only C++23 library.

**Core type (`TrustNode`):**
```c
typedef struct __attribute__((aligned(32))) {
    uint8_t  nodeid;        // 1 byte  — peer identifier
    uint32_t trust;         // 4 bytes — score (starts at 20)
    uint32_t acks;          // 4 bytes — positive interactions
    uint32_t nacks;         // 4 bytes — negative interactions
    uint8_t  reserved[19];  // padding to 32-byte alignment
} TrustNode;
```

Trust starts at 20; `trust_node_update_trust()` adjusts it based on `AckType`.

---

### Layer 3 — nodeEngine (`peer/nodeEngine/`)

**Purpose:** The living peer daemon. Hosts a type-erased async task system that will schedule and execute all peer subsystem work (mining threads, network I/O, trust updates, chain sync).

**Runtime architecture:**

```
TaskSystem
  │
  ├── TaskQueue         — MPSC deque, std::any typed payloads
  ├── Scheduler         — (stub, future: priorities + worker affinity)
  └── TaskHandle        — typed future, returned to caller
```

**Key design rules enforced in code (from `main.cpp` comments):**
- `Task` is the only container for work; raw lambdas cannot bypass the queue
- `TaskHandle` is the only typed access point to results
- `std::move` semantics are abstracted — callers pass values, not references
- `submit(Task)` is `= delete`; callers must use typed `submit(input, lambda)`

**IPv6 subsystem (`classes/ipv6/`, `classes/ipv6_pack/`):**
- `ipv6` — wraps a `std::array<uint8_t,16>`, parses string form, classifies address type (loopback / multicast / link-local / global)
- `ipv6_pack` — a vector of `ipv6` addresses with helpers to filter to global-routable set and select the primary global address

---

### Layer 4 — Frontend (`peer/frontend/`)

**Purpose:** Native desktop control-plane dashboard.

| Property | Detail |
|---|---|
| Tech | C++17, Qt6 (Qt5 fallback), QML/Quick |
| UI style | Glassmorphism dark theme — "Dark Neural Grid" |
| State | Single-source state store + diff engine |
| Polling | Adaptive async REST polling (2s – 5s intervals) |
| Network | `QNetworkAccessManager` against Bootstrapper REST API |

---

## 3. Cryptography Summary

| Primitive | Usage | Library |
|---|---|---|
| Ed25519 | Block signing / identity | OpenSSL EVP |
| X25519 | Key agreement for encryption | OpenSSL EVP |
| AES-256-GCM | Keypair storage at rest | OpenSSL EVP |
| SHA-256 | Hashing, PoW challenges, key derivation | OpenSSL |
| HMAC-SHA-256 | Rolling packet authentication | OpenSSL HMAC |

**Encrypted file format (wallet keys, chain state):**
```
[ magic(4) ][ salt(16) ][ iv(12) ][ tag(16) ][ ciphertext ]
Key = SHA256( password ‖ salt )
```

---

## 4. Platform Constraints

- **Linux-only.** Both `pkcertchain_config.h` and `TrustNode.h` contain hard `#error` guards rejecting non-Linux builds.
- **C11 / C++23** — no pre-C11 C; C++ uses structured bindings, `std::any`, concepts-level patterns.
- **Big-endian on the wire.** All serialized integers are network byte order.
- **Zero-copy philosophy.** Kernel/user-space boundary passes buffer pointers; copies only when structurally necessary.

---

## 5. Build System

```
cmake -S . -B build                          # default (nodeEngine only)
cmake -S . -B build -DPEER_BUILD_TESTS=ON    # + all unit test suites
cmake -S . -B build -DPEER_BUILD_FRONTEND=ON # + Qt frontend
cmake --build build --parallel $(nproc)
```

Outputs:
```
build/bin/nodeEngine
build/lib/libPKCertChain.a
build/lib/libRollingSignatures.a
```

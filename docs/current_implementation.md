# DATAEXSYS-P2P — Current Implementation State

> Status snapshot as of: 2026-05-12
> Build: `cmake -S . -B build && cmake --build build` ✅ Clean

---

## Summary Table

| Component | Status | Notes |
|---|---|---|
| CMake unified build | ✅ Complete | Single `cmake -S . -B build` builds all libs + nodeEngine |
| PKCertChain (identity chain) | 🟡 Headers complete, runtime stub | `src/main.c` is a placeholder |
| RollingSignatures (HMAC routing) | ✅ Core API complete + tested | 5-hop tamper test passes |
| LocalTrustDiaries (trust ledger) | 🟡 Data structure defined | `update_trust()` body is empty |
| nodeEngine (task runtime) | 🟡 Runtime functional, not integrated | No peer subsystems wired yet |
| frontend (Qt dashboard) | 🟡 UI complete, backend wiring partial | Not in default build |
| Bootstrapper (Java registry) | ✅ Spring Boot complete | Separate Maven build |

---

## PKCertChain (`peer/backend/PKCertChain/`)

### ✅ Implemented

#### Data Structures
- `uint256` — 256-bit LE word; big-endian serialization/deserialization
- `uint512` — 512-bit type; serialization/deserialization
- `certificate` — `pubSignKey` + `pubEncKey` + `id`; hash + sign helpers
- `block` — cert, cert hash, prev hash, verifier sig, height, timestamp, tier, `MiniPowResult`, `tier_pow_solve_t`; full canonical serialization
- `PKCertChain` — fixed-size block array, `index`, `NetworkName`, `complexity`, `next_challenge_id`, `avg_solve_time_seconds`, per-tier tracking indices and complexities

#### MiniPoW (Capability Classification)
- Deterministic challenge hash from serialized block fields (`challenge_id` is `uint64_t`)
- Brute-force nonce solver with `clz256` leading-zero difficulty check
- `isValidChallenge` verifier
- Fixed-size session + queue arrays
- `mini_pow_session_t` — issued/received timestamps + `target_index`

#### TierPoW (Tier-based Mining)
- Deterministic challenge including tier in hash buffer
- Brute-force nonce solver
- Session + queue arrays identical in structure to MiniPoW
- `PowManager` — orchestrates full challenge generation and solving loop
- Bayesian complexity scaling, clamped to `[1, 255]`, 600 s target per tier

#### Cryptography
- `hash256_buffer` — SHA-256 wrapper
- `clz256` — difficulty check
- Ed25519 sign (`sign_buffer_ed25519`) + verify (bool + status variants)
- `GenerateSignKeys` — Ed25519 keypair generation
- `GenerateEncKeys` — X25519 keypair generation
- `LocalSaveEncrypt` / `LocalSaveDecrypt` — AES-256-GCM with format `[magic(4)][salt(16)][iv(12)][tag(16)][ciphertext]`

#### Wallet & OS Helpers
- `WalletSetup` — creates `~/.pkcertchain/<network>/wallet` (mode `0700`)
- `LinuxUtils` — `save_sign_keys`, `save_enc_keys`, `load_sign_keys`, `load_enc_keys`; `save_chain_state`, `load_chain_state` (with SHA-256 integrity trailer)
- `OpStatus_t` — `OP_SUCCESS`, `OP_NULL_PTR`, `OP_BUFFER_TOO_SMALL`, `OP_INVALID_INPUT`, `OP_NEEDS_PRIVILEGE`, `OP_SIGN_VERIFIED_TRUE/FALSE`, `OP_INVALID_STATE`

### 🔴 Not Yet Implemented

| Item | Notes |
|---|---|
| `src/main.c` | Placeholder — prints hello, does not exercise any core logic |
| Event-driven runtime | Event loop + MPSC queues planned; not started |
| Persistent chain storage | Single-file snapshot only; no reorg, no mempool |
| Full chain validation | Only prev-block check currently; no full validation pass |
| P2P transport | No networking layer; no socket code |
| GUI wallet integration | No password input, no reward ledger |
| Unit tests | Test binaries exist in `tests/` but are mostly empty stubs |

---

## RollingSignatures (`peer/backend/RollingSignatures/`)

### ✅ Implemented

All logic is inline in `inc/hmac_routing.h`:

| Function | Status |
|---|---|
| `derive_key(k_prev, node_id, k_out)` | ✅ `Kᵢ = SHA256(K_{i-1} ‖ NodeID_i)` |
| `compute_hmac(pkt, key, out)` | ✅ HMAC-SHA256 over `payload ‖ path_vector ‖ nonce` |
| `create_packet(pkt, payload, len, k0, nonce)` | ✅ Source-side packet initialisation |
| `forward_packet(pkt, node_id, k_prev, k_out)` | ✅ Verify → Append → Derive → Recompute HMAC |
| `verify_packet(pkt, k0)` | ✅ Destination full key-chain replay and HMAC confirm |

**Test:** `tests/test_hmac_routing.c` — 5-node hop simulation (`11→22→33→44→55`), including tamper detection (mutating Hop 3 from `33` to `99` correctly collapses the chain and forces rejection). ✅ Passes.

### 🔴 Not Yet Implemented

| Item | Notes |
|---|---|
| Replay window | Nonce exists in packet but no nonce-tracking state |
| Integration with nodeEngine | Not connected to any routing or transport layer |

---

## LocalTrustDiaries (`peer/backend/LocalTrustDiaries/`)

### ✅ Implemented

- `TrustNode` struct: `nodeid` (u8), `trust` (u32, default 20), `acks` (u32), `nacks` (u32)
- `trust_node_init`, `trust_node_get_*`, `trust_node_set_*` — all inline
- `AckType.h` — enum/type defined
- `Size_Offsets.h` — size constants

### 🔴 Not Yet Implemented

| Item | Notes |
|---|---|
| `trust_node_update_trust()` | Body is empty (`{ }`) — core scoring logic missing |
| Trust decay / ageing | No time-based trust decay |
| Persistence | No save/load for trust state |
| Integration with nodeEngine | Not connected to any routing decision |

---

## nodeEngine (`peer/nodeEngine/`)

### ✅ Implemented

**Task Runtime:**
- `Task` — type-erased `std::any` payload + `std::function<std::any(std::any)>` executor + `std::promise<std::any>`
- `TaskQueue` — MPSC deque; enqueue / dequeue with mutex/condvar
- `TaskHandle` — typed `.get<T>()` wrapper over `std::future<std::any>`
- `TaskSystem` — `start()` / `stop()` / `submit(input, lambda)` / `submit(lambda)` (no-input)
- `Scheduler` — stub class, no logic yet

**IPv6 Utilities:**
- `ipv6` — 16-byte address; parse from string, classify (loopback / multicast / link-local / global), `to_string()`
- `ipv6_pack` — `add()`, `global_addresses()`, `primary_global()` (throws if none)

**Certificate class stub** (`classes/certificate/certificate.h`) — declared, not yet implemented.

**Current `main.cpp`:** Demonstrates the task runtime only — submits two tasks (one with `tuple<int,int,int>` input, one returning a `string`), prints results, and exits.

### 🔴 Not Yet Implemented

| Item | Notes |
|---|---|
| Peer subsystem wiring | No PKCertChain / RollingSignatures calls from nodeEngine |
| Network I/O | No socket code, no IPv6 bind/listen |
| P2P event loop | main.cpp exits after demo; no persistent loop |
| Chain sync | No block broadcast or receive |
| Trust integration | LocalTrustDiaries not queried during routing |
| Scheduler implementation | `Scheduler` is an empty stub |

---

## Frontend (`peer/frontend/`)

### ✅ Implemented
- QML "Dark Neural Grid" dashboard (glassmorphism theme)
- State store + diff engine
- Adaptive REST polling (2 – 5 s)
- `QNetworkAccessManager` integration against Bootstrapper API

### 🔴 Not Yet Implemented
- Direct P2P node status (polls Bootstrapper only, not nodeEngine)
- Local nodeEngine IPC / socket bridge

---

## Bootstrapper (`Bootstrapper/`)

### ✅ Complete
- Spring Boot 3.4, Java 21
- MySQL 8 + Flyway migrations
- REST API for peer registration, counts, and bootstrap URLs
- Docker deployment configuration

---

## Build Health

```
cmake -S . -B build   →  Configures cleanly ✅
cmake --build build   →  All targets build cleanly ✅

Warnings only (not errors):
  - OpenSSL 3.0 deprecation: HMAC_CTX_*, SHA256_Init/Update/Final (legacy API usage in submodule headers)
  - PKCertChain: const qualifier on return type (cert_get_id)
```

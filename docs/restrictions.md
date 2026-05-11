# DATAEXSYS-P2P — Strict Architectural Restrictions

> These rules are **non-negotiable**. They are derived from invariants already
> enforced in source code, documented in submodule docs, and embedded in the
> existing cmake structure. Violating them breaks correctness, security, or
> build reproducibility.

---

## 1. Platform

| Rule | Source |
|---|---|
| **Linux only.** All components target Linux exclusively. | `pkcertchain_config.h`: `#error` on non-Linux; `TrustNode.h`: same guard |
| **No Windows / macOS code paths.** Do not add `#ifdef _WIN32` or `__APPLE__` fallbacks. | Both core libraries enforce `#if !defined(__linux__)` at the top of their primary headers |

---

## 2. Language Standards

| Component | Standard | Extensions |
|---|---|---|
| PKCertChain | C11 | OFF — `CMAKE_C_EXTENSIONS OFF` |
| RollingSignatures | C11 | OFF |
| LocalTrustDiaries | C++23 | OFF |
| nodeEngine | C++23 | OFF |
| frontend | C++17 | OFF |

**Rule:** Never relax the standard. If a feature requires a newer standard, upgrade the whole component deliberately — do not use compiler extensions as a workaround.

---

## 3. Serialization

> Source: `PKCertChain/docs/architecture.md` §2, §7

- **All integers on the wire must be in network byte order (big-endian).** This applies to every field in every struct that is hashed, signed, or transmitted.
- **Never hash a native struct directly.** Always hash the serialized buffer.
- **Struct fields must be explicit in layout.** Use `__attribute__((aligned(N)))` and explicit `reserved[]` padding. Never rely on compiler padding for protocol structs.
- **Zero-copy.** Kernel passes buffer pointers to user-space. Do not copy unless structurally unavoidable.

---

## 4. Cryptography

> Source: `PKCertChain/docs/architecture.md` §4, §7; `RollingSignatures/docs/architecture.md`

- **Ed25519 for signing.** Do not use RSA, ECDSA, or any non-Ed25519 signing scheme.
- **X25519 for key agreement.** Do not substitute.
- **AES-256-GCM for data at rest.** Key derivation must be `SHA256(password ‖ salt)`. Format must be `[magic(4)][salt(16)][iv(12)][tag(16)][ciphertext]`.
- **HMAC-SHA-256 for packet authentication** (RollingSignatures). Do not use truncated HMAC or a different hash.
- **All crypto logic lives in `include/util/` (PKCertChain) or `inc/hmac_routing.h` (RollingSignatures).** Do not place crypto calls in `main.c`, `main.cpp`, or any routing / scheduler layer. (Source: `PKCertChain/docs/MainNodeRoadmap.md` — "What to Avoid in main.c")
- **No hardcoded keys or secrets anywhere in source.**

---

## 5. PoW Consensus

> Source: `PKCertChain/docs/architecture.md` §5, §8; `PKCertChain/docs/CurrentImplementation.md` §7

- **Two-phase mandatory flow:** MiniPoW classification *must* precede TierPoW mining. Block addition is only permitted via the TierPoW path. Do not shortcut.
- **Difficulty is per-tier.** Each tier (`MCU`, `Edge`, `Desktop`, `Server`) maintains its own independent complexity value. A single global complexity is forbidden.
- **Bayesian difficulty updates only.** Complexity is updated via mathematically bounded Bayesian inference — not by simple +1 / -1 steps. Clamped hard to `[1, 255]`.
- **Target interval is 600 seconds.** Do not change without a documented, coordinated network upgrade.
- **`challenge_id` is `uint64_t`.** Do not downcast.

---

## 6. Wallet & Key Storage

> Source: `PKCertChain/docs/architecture.md` §4; `PKCertChain/docs/CurrentImplementation.md` §6

- **Wallet directory permissions: `0700`.** Never create with world-readable permissions.
- **Key file permissions: `0600`.** No group read. No world read.
- **Chain state file must include an appended SHA-256 integrity hash.** `load_chain_state` must verify it.
- **Wallet path:** `~/.pkcertchain/<network>/wallet` — do not deviate from this structure.

---

## 7. RollingSignatures — HMAC Chain Rules

> Source: `RollingSignatures/docs/architecture.md`

- **The HMAC is always computed over `payload ‖ path_vector ‖ nonce`.** Changing the order or omitting a field silently breaks tamper detection.
- **Key derivation is `Kᵢ = SHA256(K_{i-1} ‖ NodeID_i)`.** Do not reorder or modify the concatenation.
- **In-place HMAC overwrite only.** `forward_packet` overwrites the existing HMAC field. Do not append or allocate new fields per hop.
- **Destination performs full key-chain replay** from `K₀`. Shortcuts that skip intermediate derivation are invalid.
- **Nonce is mandatory.** A packet without a nonce must be rejected (replay protection).

---

## 8. LocalTrustDiaries

- **Trust starts at 20.** Do not initialise to 0 or 100.
- **`TrustNode` is 32-byte aligned** (`__attribute__((aligned(32)))`). Do not remove the alignment attribute.
- **`trust_node_update_trust()` must accept an `AckType` — not a raw integer.** Type safety is enforced by the header.

---

## 9. nodeEngine — Task System Rules

> Source: `peer/nodeEngine/src/main.cpp` inline comments (RULE annotations)

| Rule # | Rule |
|---|---|
| RULE 4 | `TaskHandle` is the **only** typed access point to task results. Do not access task internals directly. |
| RULE 10 | `std::move` logic and `Task` creation are abstracted inside `submit()`. Callers pass values, not `Task` objects. |
| Enforced by `= delete` | `submit(Task task)` is deleted. Raw `Task` objects cannot be submitted by callers — only typed lambdas. |

- **Do not put business logic in the scheduler.** The scheduler manages *when* tasks run; subsystem logic belongs in the tasks themselves.
- **Do not block inside a task.** Tasks must be non-blocking; blocking calls must be wrapped with async continuations.

---

## 10. cmake Build System

- **Single entry point: root `CMakeLists.txt` only.** Do not instruct users to `cd peer && cmake .`.
- **All targets are namespaced `PEER::<Name>`.** Direct target names (`PKCertChain`, `RollingSignatures`, etc.) exist for the linker; consumer code links via `PEER::` aliases.
- **`PEER::Flags` must be linked by every executable target** to inherit the project-wide `-Wall -Wextra -Wpedantic` diagnostic set.
- **`cmake/backend/main.cmake` is the single point of `find_package` calls for OpenSSL and Threads.** Do not call `find_package(OpenSSL)` in individual sub-cmake files — they include `main.cmake` instead.
- **`CONFIGURE_DEPENDS` is required on all `GLOB_RECURSE` calls.** CMake must re-run configure when source files are added or removed.
- **No `include_directories()` or `link_directories()` (directory-scope commands).** Use only `target_include_directories()` and `target_link_libraries()` with explicit scoping (`PUBLIC` / `PRIVATE` / `INTERFACE`).

---

## 11. What Must Never Live in `main.c` / `main.cpp`

> Source: `PKCertChain/docs/MainNodeRoadmap.md` — "What to Avoid"

- No crypto logic — belongs in `include/util/` (PKCertChain) or `inc/` (RollingSignatures)
- No block validation — belongs in `PowManager` and `block.h`
- No hardcoded IP addresses — use peer-discovery file or DNS seeds
- No global mutable state accessed without synchronisation
- No blocking calls on the main thread once the event loop is running

---

## 12. Submodule Discipline

- **Submodule paths are canonical:** `peer/backend/PKCertChain`, `peer/backend/RollingSignatures`, `peer/backend/LocalTrustDiaries`. Do not move them without updating `.gitmodules` **and** all `cmake/backend/*.cmake` files simultaneously.
- **Do not commit submodule source files as regular tracked files.** They must remain as gitlinks (submodule references).
- **After restructuring submodule paths**, always run `git submodule sync && git submodule update --init --recursive` to reconcile the local `.git/config` with `.gitmodules`.

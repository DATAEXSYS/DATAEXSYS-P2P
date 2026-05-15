# DATAEXSYS-P2P Class Diagrams

This document contains Mermaid class diagrams mapping out the key structures and classes within the `shared` and `nodeEngine` directories.

## 1. nodeEngine: Task Runtime System

The `nodeEngine` folder primarily implements a multithreaded task execution runtime based on type-erased payloads.

```mermaid
classDiagram
    class TaskSystem {
        +start() void
        +stop() void
        +submit(input, lambda) TaskHandle
        +submit(lambda) TaskHandle
    }

    class TaskQueue {
        -std::deque queue
        -std::mutex mtx
        -std::condition_variable cv
        +enqueue(Task) void
        +dequeue() Task
    }

    class Task {
        -std::any payload
        -std::function executor
        -std::promise promise
        +execute() void
    }

    class TaskHandle {
        -std::future future_result
        +get~T~() T
        +wait() void
    }

    class Scheduler {
        <<stub>>
        +schedule()
    }

    class Input~T~ {
        +T data
    }

    class Output~T~ {
        +T data
    }
    
    class NoInput {
        <<struct>>
    }
    
    class NoOutput {
        <<struct>>
    }

    TaskSystem --> TaskQueue : manages
    TaskQueue o-- Task : holds
    TaskSystem ..> TaskHandle : returns
    TaskHandle --> Task : wraps future
```

---

## 2. Shared: Blockchain & Cryptography (`peer/shared/inc/protocol/blockchain` & `crypto`)

These structures define the identity chain and block verification data.

```mermaid
classDiagram
    class uint256_t {
        <<struct>>
        +uint8_t[32] data
    }

    class uint512_t {
        <<struct>>
        +uint8_t[64] data
    }

    class certificate_t {
        <<struct>>
        +uint256_t pubSignKey
        +uint256_t pubEncKey
        +uint64_t id
    }

    class block_t {
        <<struct>>
        +certificate_t cert
        +uint256_t cert_hash
        +uint256_t prev_hash
        +uint512_t verifier_sig
        +uint64_t height
        +uint64_t timestamp
        +uint8_t tier
        +mini_pow_solve_t MiniPowResult
        +tier_pow_solve_t TierPowResult
    }

    class PKCertChain_t {
        <<struct>>
        +block_t[] blocks
        +uint64_t index
        +char[] NetworkName
        +uint32_t complexity
        +uint64_t next_challenge_id
        +uint32_t avg_solve_time_seconds
    }

    block_t *-- certificate_t : contains
    block_t *-- uint256_t : uses
    block_t *-- uint512_t : uses
    PKCertChain_t o-- block_t : maintains
```

---

## 3. Shared: Trust & Networking (`peer/shared/inc/protocol`)

These definitions represent the P2P networking layer and local trust ledger tracking.

```mermaid
classDiagram
    class ipv6_t {
        <<struct>>
        +uint8_t[16] addr
    }

    class TrustNode_t {
        <<struct>>
        +uint8_t nodeid
        +uint32_t trust
        +uint32_t acks
        +uint32_t nacks
    }

    class RoutingPacket_t {
        <<struct>>
        +PacketHeader_t header
        +uint8_t[] payload
        +uint256_t hmac
    }

    class PacketHeader_t {
        <<struct>>
        +uint8_t version
        +uint8_t hop_count
        +ipv6_t source
        +ipv6_t destination
    }

    class PacketCreationContext {
        <<struct>>
        +ipv6_t src
        +ipv6_t dst
        +uint8_t[] data
    }
    
    class NodeCacheEntry_t {
        <<struct>>
        +ipv6_t address
        +uint64_t last_seen
    }

    RoutingPacket_t *-- PacketHeader_t : header
    PacketHeader_t *-- ipv6_t : addressing
```

---

## 4. Shared: Proof-of-Work (Tier & Mini)

Structures mapping out the mathematical gating and validation components.

```mermaid
classDiagram
    class PowManager_t {
        <<struct>>
        +tier_pow_queue_t queue
        +tier_pow_session_t[] sessions
    }

    class tier_pow_challenge_t {
        <<struct>>
        +uint64_t challenge_id
        +uint256_t target_hash
        +uint8_t tier
    }

    class tier_pow_solve_t {
        <<struct>>
        +uint64_t nonce
        +uint256_t result_hash
    }

    class mini_pow_challenge_t {
        <<struct>>
        +uint64_t challenge_id
        +uint256_t target_hash
    }

    class mini_pow_solve_t {
        <<struct>>
        +uint64_t nonce
    }
    
    class mini_pow_session_t {
        <<struct>>
        +uint64_t issued_timestamp
        +uint64_t received_timestamp
        +uint32_t target_index
    }

    PowManager_t --> tier_pow_challenge_t : issues
    PowManager_t --> tier_pow_solve_t : verifies
```

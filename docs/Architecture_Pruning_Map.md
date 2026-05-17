# DATAEXSYS-P2P: Architecture Pruning & Scale Survival Map

Based on the meta-evaluation, the system's core limitation is not its logical correctness, but the **interference between Layer 1 (Reactive Routing) and Layer 2 (Security/Trust)** under heavy load. To survive 1000–10,000 nodes, the system must decouple the "Security-Latency-Scalability" triangle.

Here is the blueprint for pruning the architecture and unblocking the critical execution path.

---

## 1. The Critical Path Decoupling (What to move to background)

Currently, `nodeEngine` processes routing (`RREQ`/`RREP`), identity verification (Ed25519), and Proof-of-Work (MiniPoW) sequentially on the same critical path. 

**Action Plan:**
- **Separate Data Plane and Control Plane Queues:** Create two distinct task queues in `nodeEngine`. 
  - *Data Plane (High Priority):* Pure packet forwarding and basic DSR routing. No heavy cryptography allowed on this thread pool.
  - *Control Plane (Background):* Ed25519 signature verification, PoW validation, and Trust Diary updates.
- **Lazy Verification:** When a new `RREQ` arrives, forward it immediately if the peer is temporarily "grey-listed" (provisionally trusted), and offload the Ed25519/PoW validation to the background. If the background task fails, drop the route and penalize the trust score asynchronously.

---

## 2. Cryptographic Placement Optimization (What to redesign)

The previous evaluation suggested ripping out per-hop HMACs. A more nuanced, academically defensible approach is **selective or batched validation**.

**Action Plan:**
- **Probabilistic HMAC Verification:** Instead of verifying the Rolling HMAC chain at *every single hop*, intermediate nodes verify the HMAC with a probability $p$ (e.g., 10%). 
  - *Result:* Reduces per-hop CPU overhead by 90% while still maintaining a high statistical probability of catching Blackhole/Wormhole attackers. The destination node still performs a 100% full chain validation.
- **Batching Ed25519 Signatures:** Group identity verifications into time-slices (e.g., every 500ms) rather than processing them per-packet, allowing SIMD or optimized batch-verification cryptography to be used.

---

## 3. Broadcast Suppression (Surviving Scale)

To prevent the reactive routing broadcast pressure from causing a probabilistic scaling failure at 1000+ nodes:

**Action Plan:**
- **RREQ Aggregation & Damping:** Nodes must track recently seen `RREQ` hashes and suppress duplicates dynamically.
- **TTL & Radius Tuning:** Restrict the broadcast radius of initial discoveries, expanding only if the target is not found within $N$ hops.
- **Trust-Weighted Broadcasts:** Only forward `RREQ` floods to peers with a `LocalTrustDiary` score above a certain threshold, isolating spammy nodes from the core routing backbone.

---

## 4. Hardware Mismatch & PoW Calibration

To address the "PoW freezes IoT" critique without completely abandoning Sybil resistance:

**Action Plan:**
- **Delegated Proof-of-Work:** Allow low-power nodes to request a high-power trusted neighbor to solve the TierPoW challenge on their behalf, exchanging "Trust" points for CPU cycles.
- **Dynamic Difficulty Calibration:** Tie the PoW challenge difficulty not just to the tier, but to the hardware classification *and* the node's current trust score. A highly trusted mobile node should face almost zero PoW resistance, while an untrusted new node faces a steeper curve.

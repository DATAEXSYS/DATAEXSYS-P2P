# DATAEXSYS-P2P: Threat Model, Evaluation Plan, & Simulation Failure Modes

This document serves as the formal evaluation layer for the DATAEXSYS-P2P system, detailing the threat model, proposed empirical metrics, and the predicted failure modes under real-world deployment simulations.

---

## 1. Threat Model Definition

To rigorously evaluate the security claims of the protocol, we define the following threat landscape:

### Attacker Profile
- **Type:** Insider threat (malicious nodes participating legitimately within the IPv6 overlay).
- **Resources:** Consumer-grade hardware up to medium-scale botnets (Sybil). The attacker does *not* possess quantum computing or ISP-level global network surveillance capabilities.

### Attacker Capabilities
- **Can:** Drop packets (Blackhole), falsely advertise routes (Wormhole), spoof hardware classifications, delay traffic, and generate high volumes of connection requests.
- **Cannot:** Break Ed25519/X25519 cryptography, forge Rolling HMAC signatures without the derivation chain, or bypass the Bootstrapper registry arbitrarily.

### Out of Scope (Currently Unmitigated)
- **Eclipse Attacks:** Encircling a target node such that all its neighbors are malicious.
- **Global Traffic Analysis:** The protocol secures payloads and paths, but does not currently implement traffic padding to obscure communication patterns from ISPs.

---

## 2. Evaluation Plan & Baseline Metrics

To quantify the tradeoffs of the DATAEXSYS-P2P architecture, performance must be measured against a defined baseline rather than theoretical maximums.

### Baseline Comparison
The system will be benchmarked against **standard unencrypted IPv6 UDP routing** and **basic libp2p pubsub (GossipSub)** operating under identical network conditions.

### Core Metrics to Capture
1. **Discovery Latency (TTFP - Time To First Packet):** Measures the penalty of reactive DSR routing combined with PoW solving.
2. **Sustained Overlay Throughput:** Measures the cryptographic bandwidth cap imposed by per-hop Rolling HMAC derivations compared to kernel-level IP forwarding.
3. **Sybil Rejection Efficiency:** Measures the time taken for the Bayesian TierPoW difficulty to scale and successfully throttle a simulated botnet connection flood.
4. **Trust Convergence Rate:** The time required for the `LocalTrustDiaries` to correctly isolate a node exhibiting 50% packet-drop behavior.

---

## 3. Operating Regime Limitations

The protocol is designed to operate stably within specific constraints:
- **Optimal Regime:** Small to medium networks (100–1000 active concurrent nodes) with **low-to-medium churn** (nodes remaining connected for >30 minutes).
- **Suboptimal Regime:** Highly volatile networks (mobile ad-hoc) where routes break rapidly, forcing constant re-execution of the expensive DSR + PoW + Ed25519 handshake.

---

## 4. Simulation Failure Modes: What Breaks First?

In a real-world deployment simulation (or a rigorous thesis viva defense), the system will fail under extreme stress. We predict the following cascading failures based on the architectural design:

### Scenario A: High Churn (The "Storm" Failure)
*Condition: Nodes are joining and dropping from the overlay every 5–10 seconds.*
- **What breaks first:** The **TaskQueue (CPU Starvation)**.
- **Mechanism:** Because Ed25519 signature verification and MiniPoW solving are on the critical path for establishing trust, a high churn rate will flood the `nodeEngine` MPSC queue with handshake tasks. The CPU will spend 100% of its cycles verifying nodes that immediately disconnect. 
- **Consequence:** The thread pool saturates. Legitimate, established DSR routing packets sitting in the queue are delayed or dropped, causing false RERR (Route Error) broadcasts and destroying the network's throughput.

### Scenario B: High Throughput (The "Bandwidth" Failure)
*Condition: Two established nodes attempt a massive, sustained file transfer.*
- **What breaks first:** The **Rolling HMAC Forwarding Pipeline**.
- **Mechanism:** Standard routers forward packets using specialized hardware in nanoseconds. In DATAEXSYS, every intermediate node must compute $K_i = SHA256(K_{i-1} \parallel NodeID_i)$ and an HMAC over the payload in user-space. 
- **Consequence:** The forwarding nodes' CPUs will max out computing hashes. The overlay network's maximum sustained throughput will plummet, completely constrained by the single-core cryptographic speed of the weakest intermediate node in the path.

### Scenario C: Selective Routing Manipulation (The "Hardware Spoof" Failure)
*Condition: An attacker simulates high-tier hardware (e.g., Server class) to bypass strict routing.*
- **What breaks first:** The **Local Trust Diaries (Traffic Siphoning)**.
- **Mechanism:** The attacker lies about their hardware class to attract optimal route requests. Once they are placed on the critical path for multiple routes, they *selectively* drop 10% of packets.
- **Consequence:** Because DSR relies on binary path failures, and `LocalTrustDiaries` penalizes drops locally, the network lacks the global consensus to rapidly blacklist the node. The attacker successfully degrades global network quality while remaining just trusted enough to stay in the routing tables.

# DATAEXSYS — Decentralized Authenticated Transfer and Access Exchange SYStem

![DATAEXSYS Logo](https://img.shields.io/badge/DATAEXSYS-P2P-4DA3FF?style=for-the-badge)
![Qt](https://img.shields.io/badge/Qt-5.15+-41CD52?style=for-the-badge&logo=qt)
![Spring Boot](https://img.shields.io/badge/Spring_Boot-3.4.1-6DB33F?style=for-the-badge&logo=springboot)
![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B)

**DATAEXSYS** is a high-performance, decentralized networking platform designed for autonomous IPv6 mesh connectivity. It features a modern QML-based control interface and a robust Spring Boot discovery backend (Global Node), providing a "SaaS-level" experience for decentralized systems.

---

## 🌌 Vision

To bridge the gap between complex peer-to-peer networking and user-friendly control systems. DATAEXSYS provides a reactive, real-time dashboard for managing decentralized authenticated transfers and mesh network access.

---

## 🏗️ System Architecture

DATAEXSYS is split into two primary layers to ensure scalability and ease of discovery.

### 1. Global Node (Bootstrapper)
A centralized (or federated) registry that solves the "initial peer" problem.
*   **Tech**: Java 21, Spring Boot, MySQL, Flyway.
*   **Role**: Manages network metadata, active peer counts, and bootstrap URLs.
*   **API**: RESTful interface for discovery and peer updates.

### 2. Peer Frontend (Control Plane)
A native desktop application providing a "Dark Neural Grid" dashboard.
*   **Tech**: C++17, Qt5 (QML/Quick), QNetworkAccessManager.
*   **Architecture**:
    *   **State Store**: Single source of truth for the local application state.
    *   **Diff Engine**: Efficient change detection between API snapshots and local state.
    *   **Polling Manager**: Adaptive async polling (2s - 5s intervals based on activity).
    *   **QML UI**: Modern, glassmorphism-based reactive interface.

---

## 📂 Project Structure

```text
DATAEXSYS-P2P/
├── Bootstrapper/              # Global Node Backend (Java/Spring)
│   ├── src/                   # Service, Controller, and Repository logic
│   └── Dockerfile             # Containerized deployment config
├── peer/
│   └── frontend/              # Native Desktop Client (C++/Qt)
│       ├── include/backend/   # State, Polling, and API headers
│       ├── qml/               # Modern QML UI (Pages, Components, Theme)
│       ├── src/backend/       # Event-driven logic implementation
│       └── CMakeLists.txt     # Build configuration
└── README.md
```

---

## ⚡ Key Features

*   **Adaptive Polling**: Simulates real-time updates over REST by intelligently adjusting polling frequency based on network activity.
*   **Reactive UI**: UI components only re-render when specific data diffs are detected in the state store.
*   **Glassmorphism Design**: A premium, futuristic "infra dashboard" aesthetic with smooth transitions and glow effects.
*   **IPv6 Ready**: Designed for decentralized mesh networking architectures.
*   **Event-Driven**: Fully asynchronous non-blocking architecture ensures the UI remains fluid under heavy load.

---

## 🚀 Getting Started

### Prerequisites
*   **Backend**: Java 21+, MySQL 8.
*   **Frontend**: Qt 5.15+ (with Quick/Qml modules), CMake 3.5+.

### Build & Run (Frontend)
```bash
cd peer/frontend
mkdir build && cd build
cmake ..
make -j$(nproc)
./P2PFrontend
```

### Run (Backend)
```bash
cd Bootstrapper
./mvnw spring-boot:run
```

---

## 🛠️ Roadmap

- [x] **Phase 1**: Bootstrap Layer Architecture (REST + State Management).
- [x] **Phase 2**: Modern QML "Dark Neural Grid" UI.
- [ ] **Phase 3**: Integration of Native IPv6 Mesh Engine.
- [ ] **Phase 4**: End-to-End Authenticated Transfer implementation.
- [ ] **Phase 5**: Real-time Topology Visualization.

---

## 📄 License
Custom Proprietary - DATAEXSYS Project.

---

> 🧠 **"Frontend behaves like a reactive system even though backend is REST-only"**
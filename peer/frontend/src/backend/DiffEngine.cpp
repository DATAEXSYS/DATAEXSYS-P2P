#include "backend/DiffEngine.h"
#include <algorithm>
#include <set>

DiffEngine::DiffEngine(StateStore *store, QObject *parent)
    : QObject(parent), m_store(store) {}

void DiffEngine::processNetworks(const std::vector<Network>& newNetworks) {
    const auto& currentNetworks = m_store->networks();
    bool changed = false;

    // 1. Detect removals
    for (const auto& current : currentNetworks) {
        auto it = std::find_if(newNetworks.begin(), newNetworks.end(), 
            [&](const Network& n) { return n.networkId == current.networkId; });
        
        if (it == newNetworks.end()) {
            emit networkRemoved(current.networkId);
            changed = true;
        }
    }

    // 2. Detect additions and updates
    for (const auto& incoming : newNetworks) {
        auto it = std::find_if(currentNetworks.begin(), currentNetworks.end(), 
            [&](const Network& n) { return n.networkId == incoming.networkId; });
        
        if (it == currentNetworks.end()) {
            emit networkAdded(incoming);
            changed = true;
        } else if (*it != incoming) {
            emit networkUpdated(incoming);
            changed = true;
        }
    }

    if (changed) {
        m_store->setNetworks(newNetworks);
        emit changeDetected();
    } else {
        emit noChangeDetected();
    }
}

void DiffEngine::processPeers(const std::vector<QString>& newPeers) {
    const auto& currentPeers = m_store->peers();
    
    if (currentPeers.size() != newPeers.size() || 
        !std::equal(currentPeers.begin(), currentPeers.end(), newPeers.begin())) {
        
        m_store->setPeers(newPeers);
        emit changeDetected();
    } else {
        emit noChangeDetected();
    }
}

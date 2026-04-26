#include "backend/StateStore.h"

StateStore::StateStore(QObject *parent) : QObject(parent) {}

void StateStore::setNetworks(const std::vector<Network>& networks) {
    m_networks = networks;
    emit networksChanged(m_networks);
}

void StateStore::setPeers(const std::vector<QString>& peers) {
    m_peers = peers;
    emit peersChanged(m_peers);
}

void StateStore::setConnected(bool connected) {
    if (m_isConnected != connected) {
        m_isConnected = connected;
        emit connectionStatusChanged(m_isConnected);
    }
}

#include "backend/AppController.h"
#include <numeric>
#include <QRandomGenerator>

AppController::AppController(QObject *parent) : QObject(parent) {
    m_networkService = new NetworkService(this);
    m_networkModel = new NetworkModel(this);
    
    auto store = m_networkService->store();

    // --- BINDING TO STATE STORE ---
    connect(store, &StateStore::networksChanged, this, &AppController::onNetworksChanged);
    connect(store, &StateStore::connectionStatusChanged, this, &AppController::onConnectionStatusChanged);
    
    // --- ERROR HANDLING ---
    connect(m_networkService, &NetworkService::errorOccurred, this, &AppController::onError);
}

void AppController::refreshNetworks() {
    // In this architecture, refresh is handled by the PollingManager.
    // We can still trigger an immediate poll if needed.
    // m_networkService->performPoll(); 
}

void AppController::createNetwork(const QString &name, const QString &type, const QString &description) {
    Network n;
    n.name = name;
    n.type = type;
    n.description = description;
    
    uint32_t randomVal = QRandomGenerator::global()->generate() % 1000;
    n.networkId = name.toLower().replace(" ", "-") + "-" + QString::number(randomVal);
    n.bootstrapUrl = "https://bootstrap.dataexsys.io/" + n.networkId;
    
    m_networkService->createNetwork(n);
}

void AppController::onNetworksChanged(const std::vector<Network> &networks) {
    // Only update the model when data actually changes (ensured by DiffEngine)
    m_networkModel->setNetworks(networks);
    
    int total = std::accumulate(networks.begin(), networks.end(), 0, [](int sum, const Network &n) {
        return sum + n.activePeers;
    });

    if (m_totalPeers != total) {
        m_totalPeers = total;
        emit totalPeersChanged();
    }
}

void AppController::onConnectionStatusChanged(bool connected) {
    if (m_isOnline != connected) {
        m_isOnline = connected;
        emit isOnlineChanged();
    }
}

void AppController::onError(const QString &message) {
    emit errorOccurred(message);
}

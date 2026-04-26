#include "backend/AppController.h"
#include <numeric>

AppController::AppController(QObject *parent) : QObject(parent) {
    m_networkService = new NetworkService(this);
    m_networkModel = new NetworkModel(this);
    
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(5000); // 5 seconds
    connect(m_refreshTimer, &QTimer::timeout, this, &AppController::refreshNetworks);
    m_refreshTimer->start();

    connect(m_networkService, &NetworkService::networksLoaded, this, &AppController::onNetworksReceived);
    connect(m_networkService, &NetworkService::errorOccurred, this, &AppController::onError);

    refreshNetworks();
}

void AppController::refreshNetworks() {
    m_networkService->fetchNetworks();
}

void AppController::createNetwork(const QString &name, const QString &type, const QString &description) {
    Network n;
    n.name = name;
    n.type = type;
    n.description = description;
    n.networkId = name.toLower().replace(" ", "-") + "-" + QString::number(qrand() % 1000);
    n.bootstrapUrl = "https://bootstrap.dataexsys.io/" + n.networkId;
    m_networkService->addNetwork(n);
}

void AppController::onNetworksReceived(const std::vector<Network> &networks) {
    m_networkModel->setNetworks(networks);
    
    int total = std::accumulate(networks.begin(), networks.end(), 0, [](int sum, const Network &n) {
        return sum + n.activePeers;
    });

    if (m_totalPeers != total) {
        m_totalPeers = total;
        emit totalPeersChanged();
    }

    if (!m_isOnline) {
        m_isOnline = true;
        emit isOnlineChanged();
    }
}

void AppController::onError(const QString &message) {
    if (m_isOnline) {
        m_isOnline = false;
        emit isOnlineChanged();
    }
    emit errorOccurred(message);
}

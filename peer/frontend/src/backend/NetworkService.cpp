#include "backend/NetworkService.h"
#include <QDebug>

NetworkService::NetworkService(QObject *parent) : QObject(parent) {
    m_apiClient = new ApiClient(this);
    m_store = new StateStore(this);
    m_diffEngine = new DiffEngine(m_store, this);
    m_pollingManager = new PollingManager(this);

    // --- PIPELINE CONNECTIONS ---

    // 1. Polling -> API Fetch
    connect(m_pollingManager, &PollingManager::tick, this, &NetworkService::performPoll);

    // 2. API Response -> Diff Engine
    connect(m_apiClient, &ApiClient::networksReceived, this, &NetworkService::onNetworksReceived);
    
    // 3. Diff Engine -> Polling Manager (Adaptive)
    connect(m_diffEngine, &DiffEngine::changeDetected, m_pollingManager, &PollingManager::onActivityDetected);
    connect(m_diffEngine, &DiffEngine::noChangeDetected, m_pollingManager, &PollingManager::onNoActivityDetected);

    // 4. API Error -> Pipeline
    connect(m_apiClient, &ApiClient::error, this, &NetworkService::onError);

    m_pollingManager->start();
    performPoll(); // Initial poll
}

void NetworkService::performPoll() {
    m_apiClient->getNetworks();
}

void NetworkService::createNetwork(const Network &network) {
    json j;
    j["name"] = network.name.toStdString();
    j["type"] = network.type.toStdString();
    j["description"] = network.description.toStdString();
    j["bootstrapUrl"] = network.bootstrapUrl.toStdString();
    j["networkId"] = network.networkId.toStdString();
    
    m_apiClient->createNetwork(j);
}

void NetworkService::updatePeerCount(const QString &networkId, int count) {
    m_apiClient->updatePeers(networkId, count);
}

void NetworkService::onNetworksReceived(json data) {
    m_store->setConnected(true);
    std::vector<Network> networks;
    if (data.is_array()) {
        for (const auto &item : data) {
            networks.push_back(parseNetwork(item));
        }
    }
    m_diffEngine->processNetworks(networks);
}

void NetworkService::onError(QString message) {
    m_store->setConnected(false);
    emit errorOccurred(message);
}

Network NetworkService::parseNetwork(const json &j) {
    Network n;
    if (j.contains("id") && !j["id"].is_null()) n.id = j["id"].get<long>();
    if (j.contains("networkId") && !j["networkId"].is_null()) n.networkId = QString::fromStdString(j["networkId"]);
    if (j.contains("name") && !j["name"].is_null()) n.name = QString::fromStdString(j["name"]);
    if (j.contains("bootstrapUrl") && !j["bootstrapUrl"].is_null()) n.bootstrapUrl = QString::fromStdString(j["bootstrapUrl"]);
    if (j.contains("description") && !j["description"].is_null()) n.description = QString::fromStdString(j["description"]);
    if (j.contains("type") && !j["type"].is_null()) n.type = QString::fromStdString(j["type"]);
    if (j.contains("active") && !j["active"].is_null()) n.active = j["active"].get<bool>();
    if (j.contains("activePeers") && !j["activePeers"].is_null()) n.activePeers = j["activePeers"].get<int>();
    return n;
}
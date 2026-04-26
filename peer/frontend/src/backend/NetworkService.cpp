#include "backend/NetworkService.h"
#include <QDebug>

NetworkService::NetworkService(QObject *parent) : QObject(parent) {
    apiClient = new ApiClient(this);

    connect(apiClient, &ApiClient::networksReceived, this, &NetworkService::onNetworksReceived);
    connect(apiClient, &ApiClient::networkReceived, this, &NetworkService::onNetworkReceived);
    connect(apiClient, &ApiClient::peersUpdated, this, &NetworkService::onPeersUpdated);
    connect(apiClient, &ApiClient::error, this, &NetworkService::onError);
}

void NetworkService::fetchNetworks() {
    apiClient->getNetworks();
}

void NetworkService::addNetwork(const Network &network) {
    json j;
    j["name"] = network.name.toStdString();
    j["type"] = network.type.toStdString();
    j["description"] = network.description.toStdString();
    j["bootstrapUrl"] = network.bootstrapUrl.toStdString();
    j["networkId"] = network.networkId.toStdString();
    
    apiClient->createNetwork(j);
}

void NetworkService::updatePeerCount(const QString &networkId, int count) {
    apiClient->updatePeers(networkId, count);
}

void NetworkService::onNetworksReceived(json data) {
    std::vector<Network> networks;
    if (data.is_array()) {
        for (const auto &item : data) {
            networks.push_back(parseNetwork(item));
        }
    }
    emit networksLoaded(networks);
}

void NetworkService::onNetworkReceived(json data) {
    emit networkUpdated(parseNetwork(data));
}

void NetworkService::onPeersUpdated(json data) {
    emit networkUpdated(parseNetwork(data));
}

void NetworkService::onError(QString message) {
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
#include "service/NetworkService.h"

NetworkService::NetworkService(QObject *parent)
    : QObject(parent)
{
    api = new NetworkApi(this);

    // ================= ALL NETWORKS =================
    connect(api, &NetworkApi::networksReceived,
            this, [=](json data) {

        cache = parseNetworks(data);
        emit networksUpdated(cache);
    });

    // ================= SINGLE NETWORK =================
    connect(api, &NetworkApi::networkReceived,
            this, [=](json data) {

        Network n = parseNetwork(data);
        emit networkUpdated(n);
    });

    // ================= ERROR =================
    connect(api, &NetworkApi::error,
            this, &NetworkService::error);
}

// ================= API CALL WRAPPERS =================

void NetworkService::loadNetworks() {
    api->getNetworks();
}

void NetworkService::loadNetwork(const QString &networkId) {
    api->getNetwork(networkId);
}

void NetworkService::createNetwork(const Network &network) {
    json j;
    j["networkId"] = network.networkId.toStdString();
    j["name"] = network.name.toStdString();
    j["bootstrapUrl"] = network.bootstrapUrl.toStdString();
    j["description"] = network.description.toStdString();
    j["type"] = network.type.toStdString();
    j["active"] = network.active;
    j["activePeers"] = network.activePeers;

    api->createNetwork(j);
}

void NetworkService::updatePeerCount(const QString &networkId, int count) {
    api->updatePeers(networkId, count);
}

// ================= CACHE =================

QVector<Network> NetworkService::getCachedNetworks() const {
    return cache;
}

// ================= JSON → MODEL =================

Network NetworkService::parseNetwork(const json &j) {
    Network n;

    n.id = j.value("id", 0);
    n.networkId = QString::fromStdString(j.value("networkId", ""));
    n.name = QString::fromStdString(j.value("name", ""));
    n.bootstrapUrl = QString::fromStdString(j.value("bootstrapUrl", ""));
    n.description = QString::fromStdString(j.value("description", ""));
    n.type = QString::fromStdString(j.value("type", ""));
    n.active = j.value("active", true);
    n.activePeers = j.value("activePeers", 0);

    // NOTE: skipping datetime parsing for now (can upgrade later)

    return n;
}

QVector<Network> NetworkService::parseNetworks(const json &j) {
    QVector<Network> list;

    for (auto &item : j) {
        list.push_back(parseNetwork(item));
    }

    return list;
}
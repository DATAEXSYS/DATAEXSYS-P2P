#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <vector>
#include "backend/ApiClient.h"
#include "models/Network.h"

class NetworkService : public QObject {
    Q_OBJECT

public:
    explicit NetworkService(QObject *parent = nullptr);

    void fetchNetworks();
    void addNetwork(const Network &network);
    void updatePeerCount(const QString &networkId, int count);

signals:
    void networksLoaded(const std::vector<Network> &networks);
    void networkUpdated(const Network &network);
    void errorOccurred(const QString &message);

private slots:
    void onNetworksReceived(json data);
    void onNetworkReceived(json data);
    void onPeersUpdated(json data);
    void onError(QString message);

private:
    ApiClient *apiClient;
    Network parseNetwork(const json &j);
};

#endif
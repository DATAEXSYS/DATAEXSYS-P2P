#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QVector>

#include "api/NetworkApi.h"
#include "model/Network.h"

class NetworkService : public QObject {
    Q_OBJECT

public:
    explicit NetworkService(QObject *parent = nullptr);

    // actions
    void loadNetworks();
    void loadNetwork(const QString &networkId);
    void createNetwork(const Network &network);
    void updatePeerCount(const QString &networkId, int count);

    // cache access
    QVector<Network> getCachedNetworks() const;

signals:
    void networksUpdated(QVector<Network> networks);
    void networkUpdated(Network network);
    void error(QString message);

private:
    NetworkApi *api;

    QVector<Network> cache;

    // converters
    Network parseNetwork(const json &j);
    QVector<Network> parseNetworks(const json &j);
};

#endif
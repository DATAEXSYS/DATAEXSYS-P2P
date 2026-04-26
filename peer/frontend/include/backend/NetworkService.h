#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <vector>
#include "backend/ApiClient.h"
#include "backend/StateStore.h"
#include "backend/DiffEngine.h"
#include "backend/PollingManager.h"
#include "models/Network.h"

class NetworkService : public QObject {
    Q_OBJECT

public:
    explicit NetworkService(QObject *parent = nullptr);

    StateStore* store() const { return m_store; }
    
    void createNetwork(const Network &network);
    void updatePeerCount(const QString &networkId, int count);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onNetworksReceived(json data);
    void onError(QString message);
    void performPoll();

private:
    ApiClient *m_apiClient;
    StateStore *m_store;
    DiffEngine *m_diffEngine;
    PollingManager *m_pollingManager;

    Network parseNetwork(const json &j);
};

#endif
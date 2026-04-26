#ifndef DIFFENGINE_H
#define DIFFENGINE_H

#include <QObject>
#include <vector>
#include "backend/StateStore.h"
#include "models/Network.h"

class DiffEngine : public QObject {
    Q_OBJECT

public:
    explicit DiffEngine(StateStore *store, QObject *parent = nullptr);

    void processNetworks(const std::vector<Network>& newNetworks);
    void processPeers(const std::vector<QString>& newPeers);

signals:
    void changeDetected();
    void noChangeDetected();
    
    // Specific change signals
    void networkAdded(const Network& network);
    void networkRemoved(const QString& networkId);
    void networkUpdated(const Network& network);

private:
    StateStore *m_store;
};

#endif

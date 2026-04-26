#ifndef STATESTORE_H
#define STATESTORE_H

#include <QObject>
#include <vector>
#include <QString>
#include "models/Network.h"

class StateStore : public QObject {
    Q_OBJECT

public:
    explicit StateStore(QObject *parent = nullptr);

    // Getters
    const std::vector<Network>& networks() const { return m_networks; }
    const std::vector<QString>& peers() const { return m_peers; }
    bool isConnected() const { return m_isConnected; }

    // Mutators (Internal use only by DiffEngine)
    void setNetworks(const std::vector<Network>& networks);
    void setPeers(const std::vector<QString>& peers);
    void setConnected(bool connected);

signals:
    void networksChanged(const std::vector<Network>& networks);
    void peersChanged(const std::vector<QString>& peers);
    void connectionStatusChanged(bool connected);

private:
    std::vector<Network> m_networks;
    std::vector<QString> m_peers;
    bool m_isConnected = false;
};

#endif

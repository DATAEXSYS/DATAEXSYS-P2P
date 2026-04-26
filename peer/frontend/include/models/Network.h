#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <QDateTime>

struct Network {
    long id = 0;
    QString networkId;
    QString name;
    QString bootstrapUrl;
    QString description;
    QString type;
    bool active = true;
    int activePeers = 0;
    QDateTime createdAt;
    QDateTime updatedAt;

    bool operator==(const Network& other) const {
        return networkId == other.networkId &&
               name == other.name &&
               bootstrapUrl == other.bootstrapUrl &&
               description == other.description &&
               type == other.type &&
               active == other.active &&
               activePeers == other.activePeers;
    }

    bool operator!=(const Network& other) const {
        return !(*this == other);
    }
};

#endif
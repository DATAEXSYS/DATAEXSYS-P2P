#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <QDateTime>

struct Network {

    long id;
    QString networkId;
    QString name;
    QString bootstrapUrl;
    QString description;
    QString type;

    bool active;
    int activePeers;

    QDateTime createdAt;
    QDateTime updatedAt;

};

#endif
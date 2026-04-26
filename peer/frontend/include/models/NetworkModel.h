#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "models/Network.h"

class NetworkModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum NetworkRoles {
        IdRole = Qt::UserRole + 1,
        NetworkIdRole,
        NameRole,
        BootstrapUrlRole,
        DescriptionRole,
        TypeRole,
        ActiveRole,
        ActivePeersRole
    };

    explicit NetworkModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setNetworks(const std::vector<Network> &networks);
    void updateNetwork(const Network &network);

private:
    std::vector<Network> m_networks;
};

#endif

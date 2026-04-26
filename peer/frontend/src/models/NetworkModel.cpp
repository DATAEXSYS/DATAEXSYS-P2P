#include "models/NetworkModel.h"

NetworkModel::NetworkModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int NetworkModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_networks.size());
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_networks.size()))
        return QVariant();

    const Network &network = m_networks[index.row()];

    switch (role) {
        case IdRole: return QVariant::fromValue(network.id);
        case NetworkIdRole: return network.networkId;
        case NameRole: return network.name;
        case BootstrapUrlRole: return network.bootstrapUrl;
        case DescriptionRole: return network.description;
        case TypeRole: return network.type;
        case ActiveRole: return network.active;
        case ActivePeersRole: return network.activePeers;
        default: return QVariant();
    }
}

QHash<int, QByteArray> NetworkModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NetworkIdRole] = "networkId";
    roles[NameRole] = "name";
    roles[BootstrapUrlRole] = "bootstrapUrl";
    roles[DescriptionRole] = "description";
    roles[TypeRole] = "type";
    roles[ActiveRole] = "active";
    roles[ActivePeersRole] = "activePeers";
    return roles;
}

void NetworkModel::setNetworks(const std::vector<Network> &networks) {
    beginResetModel();
    m_networks = networks;
    endResetModel();
}

void NetworkModel::updateNetwork(const Network &network) {
    for (size_t i = 0; i < m_networks.size(); ++i) {
        if (m_networks[i].networkId == network.networkId) {
            m_networks[i] = network;
            QModelIndex idx = index(static_cast<int>(i));
            emit dataChanged(idx, idx);
            return;
        }
    }
}

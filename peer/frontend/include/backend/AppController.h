#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "backend/NetworkService.h"
#include "models/NetworkModel.h"

class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(NetworkModel* networkModel READ networkModel CONSTANT)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
    Q_PROPERTY(int totalPeers READ totalPeers NOTIFY totalPeersChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    NetworkModel* networkModel() const { return m_networkModel; }
    bool isOnline() const { return m_isOnline; }
    int totalPeers() const { return m_totalPeers; }

    Q_INVOKABLE void refreshNetworks();
    Q_INVOKABLE void createNetwork(const QString &name, const QString &type, const QString &description);

signals:
    void isOnlineChanged();
    void totalPeersChanged();
    void errorOccurred(const QString &message);

private slots:
    void onNetworksReceived(const std::vector<Network> &networks);
    void onError(const QString &message);

private:
    NetworkService *m_networkService;
    NetworkModel *m_networkModel;
    QTimer *m_refreshTimer;
    bool m_isOnline = false;
    int m_totalPeers = 0;
};

#endif

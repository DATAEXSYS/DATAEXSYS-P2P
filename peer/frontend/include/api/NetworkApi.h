#ifndef NETWORKAPI_H
#define NETWORKAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class NetworkApi : public QObject {
    Q_OBJECT

public:
    explicit NetworkApi(QObject *parent = nullptr);

    // GET /api/networks
    void getNetworks();

    // GET /api/networks/{networkId}
    void getNetwork(const QString &networkId);

    // POST /api/networks
    void createNetwork(const json &network);

    // PUT /api/networks/{networkId}/peers?count=
    void updatePeers(const QString &networkId, int count);

signals:
    void networksReceived(json data);
    void networkReceived(json data);
    void networkCreated(json data);
    void peersUpdated(json data);
    void error(QString message);

private slots:
    void handleReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;

    QString baseUrl = "http://localhost:8080/api/networks";
};

#endif
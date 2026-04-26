#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ApiClient : public QObject {
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    void getNetworks();
    void getNetwork(const QString &networkId);
    void createNetwork(const json &network);
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

    QString baseUrl =
        "https://dataexsys-p2p-production.up.railway.app/api/networks";
};

#endif
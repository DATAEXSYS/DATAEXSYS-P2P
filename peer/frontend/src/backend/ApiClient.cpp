#include "backend/ApiClient.h"

#include <QNetworkRequest>
#include <QUrlQuery>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

// ================= GET ALL NETWORKS =================
void ApiClient::getNetworks() {
    QNetworkRequest request{QUrl(baseUrl)};

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleReply(reply);
    });
}

// ================= GET SINGLE NETWORK =================
void ApiClient::getNetwork(const QString &networkId) {
    QUrl url(baseUrl + "/" + networkId);
    QNetworkRequest request{url};

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleReply(reply);
    });
}

// ================= CREATE NETWORK =================
void ApiClient::createNetwork(const json &network) {
    QNetworkRequest request{QUrl(baseUrl)};
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");

    QByteArray body =
        QByteArray::fromStdString(network.dump());

    QNetworkReply *reply = manager->post(request, body);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleReply(reply);
    });
}

// ================= UPDATE PEERS =================
void ApiClient::updatePeers(const QString &networkId, int count) {
    QUrl url(baseUrl + "/" + networkId + "/peers");

    QUrlQuery query;
    query.addQueryItem("count", QString::number(count));
    url.setQuery(query);

    QNetworkRequest request{url};

    QNetworkReply *reply = manager->put(request, QByteArray());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleReply(reply);
    });
}

// ================= COMMON RESPONSE HANDLER =================
void ApiClient::handleReply(QNetworkReply *reply) {

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QString url = reply->url().toString();

    reply->deleteLater();

    try {
        json data = json::parse(response.toStdString());

        // ================= ROUTING =================

        if (url.endsWith("/peers")) {
            emit peersUpdated(data);
        }

        else if (url.endsWith("/networks")) {
            emit networksReceived(data);
        }

        else if (url.contains("/networks/")) {
            emit networkReceived(data);
        }

        else {
            emit networksReceived(data);
        }

    } catch (const std::exception &e) {
        emit error(QString("JSON parse error: ") + e.what());
    }
}
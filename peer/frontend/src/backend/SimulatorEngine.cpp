#include "backend/SimulatorEngine.h"
#include <QRandomGenerator>
#include <QCryptographicHash>

SimulatorEngine::SimulatorEngine(QObject *parent) : QObject(parent) {
    m_trustDb["NodeA"] = {5.0, 1.0};
    m_trustDb["NodeB"] = {4.0, 2.0};
    m_trustDb["NodeC"] = {10.0, 0.5};
    m_trustDb["NodeD"] = {2.0, 8.0};

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, &SimulatorEngine::onSimulationTick);
    m_tickTimer->start(2000);
}

QVariantList SimulatorEngine::getTrustScores() {
    QVariantList list;
    for (auto it = m_trustDb.begin(); it != m_trustDb.end(); ++it) {
        QVariantMap m;
        m["node"] = it.key();
        m["score"] = it.value().alpha / (it.value().alpha + it.value().beta);
        list.append(m);
    }
    return list;
}

QVariantList SimulatorEngine::getCertChain() {
    QVariantList list;
    for (const auto &b : m_blockchain) {
        QVariantMap m;
        m["hash"] = b.hash;
        m["timestamp"] = b.timestamp;
        list.append(m);
    }
    return list;
}

QVariantList SimulatorEngine::getRouteLatencies() {
    QVariantList list;
    // Mock latencies
    QStringList nodes = {"NodeA", "NodeB", "NodeC", "NodeD"};
    for (const auto &n1 : nodes) {
        for (const auto &n2 : nodes) {
            if (n1 != n2) {
                QVariantMap m;
                m["route"] = n1 + "->" + n2;
                m["latency"] = QRandomGenerator::global()->bounded(10, 150);
                list.append(m);
            }
        }
    }
    return list;
}

void SimulatorEngine::simulatePow() {
    QTimer::singleShot(2000, this, [this]() {
        Block b;
        b.timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        b.hash = "0000" + QString::number(QRandomGenerator::global()->generate(), 16);
        m_blockchain.append(b);
        
        QVariantMap m;
        m["hash"] = b.hash;
        m["timestamp"] = b.timestamp;
        emit blockMined(m);
    });
}

void SimulatorEngine::sendMessage(const QString &from, const QString &to, const QString &msg) {
    QVariantMap data;
    data["from"] = from;
    data["to"] = to;
    data["msg"] = msg;
    data["trust"] = m_trustDb[from].alpha / (m_trustDb[from].alpha + m_trustDb[from].beta);
    
    QString route = from;
    if (m_dsrEnabled) {
        route += " -> NodeB -> " + to;
    } else {
        route += " -> " + to;
    }
    data["route"] = route;

    if (m_rollingEnabled) {
        data["hash"] = QString(QCryptographicHash::hash(msg.toUtf8(), QCryptographicHash::Sha256).toHex()).left(8);
    }
    
    emit messageSent(data);
}

void SimulatorEngine::onSimulationTick() {
    if (!m_attackMode) return;

    // Random anomalies
    if (m_wormholeEnabled && QRandomGenerator::global()->bounded(100) < 30) {
        emit anomalyDetected("WORMHOLE detected: travel time 2ms < 50ms expected.");
        m_trustDb["NodeB"].beta += 2.0;
        emit trustUpdated();
    }
    
    if (m_trustEnabled && QRandomGenerator::global()->bounded(100) < 20) {
        m_trustDb["NodeD"].beta += 1.0;
        emit trustUpdated();
    }
}

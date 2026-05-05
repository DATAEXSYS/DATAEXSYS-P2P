#include "backend/AppController.h"
#include <QRandomGenerator>
#include <QDateTime>

AppController::AppController(QObject *parent) : QObject(parent) {
    m_timer = new QTimer(this);
    m_timer->setInterval(2000); // 2 seconds between steps in auto-run
    connect(m_timer, &QTimer::timeout, this, &AppController::nextStep);
}

void AppController::setAutoRun(bool autoRun) {
    if (m_autoRun != autoRun) {
        m_autoRun = autoRun;
        if (m_autoRun) m_timer->start();
        else m_timer->stop();
        emit autoRunChanged();
    }
}

void AppController::startEngine() {
    resetEngine();
    nextStep();
}

void AppController::nextStep() {
    if (m_executionStep >= MAX_STEPS) {
        if (m_autoRun) setAutoRun(false);
        return;
    }

    m_executionStep++;
    executeCurrentStep();
    emit executionStepChanged();
}

void AppController::resetEngine() {
    m_executionStep = 0;
    m_nodes.clear();
    m_route.clear();
    m_blocks.clear();
    m_activePacket = Packet();
    emit executionStepChanged();
    emit logEvent("SYSTEM", "Engine reset. Ready for bootstrap.");
}

void AppController::mineBlock() {
    int index = m_blocks.size();
    QString prevHash = index > 0 ? m_blocks.last().hash : "0000000000000000";
    QString hash = QString::number(QRandomGenerator::global()->generate(), 16).rightJustified(16, '0');
    
    m_blocks.append({index, hash, prevHash, QDateTime::currentDateTime().toString("HH:mm:ss")});
    emit blockMined(index, hash, prevHash);
    emit logEvent("BLOCKCHAIN", QString("Block #%1 mined. Hash: %2...").arg(index).arg(hash.left(8)));
}

void AppController::sendMessage(const QString &from, const QString &to, const QString &text) {
    emit messageSent(from, to, text);
    emit logEvent("CHAT", QString("Message from %1 to %2: %3").arg(from, to, text));
    
    // Simulate propagation
    QTimer::singleShot(1000, this, [this, text]() {
        emit messageStatusUpdated(text, "Forwarding");
        emit logEvent("CHAT", "Message being forwarded via Node B...");
    });
    
    QTimer::singleShot(3000, this, [this, text]() {
        emit messageStatusUpdated(text, "Delivered");
        emit logEvent("CHAT", "Message delivered to destination.");
    });
}

void AppController::executeCurrentStep() {
    switch (m_executionStep) {
    case 1: // NODE BOOTSTRAP
        m_nodes.append({"Node A", 0.0, false, "Joined"});
        m_nodes.append({"Node B", 0.0, false, "Joined"});
        m_nodes.append({"Node C", 0.0, false, "Joined"});
        emit logEvent("BOOTSTRAP", "Initializing network nodes...");
        for (const auto& node : m_nodes) {
            emit nodeJoined(node.id);
            emit logEvent("BOOTSTRAP", QString("Node joined: %1").arg(node.id));
        }
        break;

    case 2: // CERTIFICATE ISSUANCE
        emit logEvent("PKCERT", "Issuing cryptographic certificates...");
        for (int i = 0; i < m_nodes.size(); ++i) {
            m_nodes[i].certificateIssued = true;
            QString certId = QString("CERT-%1-OK").arg(m_nodes[i].id.right(1));
            emit certificateIssued(m_nodes[i].id, certId);
            emit logEvent("PKCERT", QString("Certificate [%1] assigned to %2").arg(certId, m_nodes[i].id));
        }
        break;

    case 3: // TRUST INITIALIZATION
        emit logEvent("TRUST", "Calculating initial reputation scores...");
        for (int i = 0; i < m_nodes.size(); ++i) {
            double score = 0.5 + (QRandomGenerator::global()->generateDouble() * 0.4);
            m_nodes[i].trustScore = score;
            emit trustUpdated(m_nodes[i].id, score);
            emit logEvent("TRUST", QString("%1 trust initialized: %2").arg(m_nodes[i].id).arg(score, 0, 'f', 2));
        }
        break;

    case 4: // ROUTE COMPUTATION
        m_route = QStringList{"Node A", "Node B", "Node C"};
        emit logEvent("ROUTE", "Computing optimal secure path (DSR-Trust)...");
        emit routeSelected(m_route);
        emit logEvent("ROUTE", "Path selected: Node A -> Node B -> Node C");
        break;

    case 5: // PACKET CREATION
        m_activePacket = {"Node A", "Node C", "Node A", 0, "Initializing"};
        emit logEvent("CRYPTO", "Generating secure payload at Source...");
        emit packetEntered(m_activePacket.source, m_activePacket.destination);
        break;

    case 6: // HOP EXECUTION
        emit logEvent("HOP", "Executing multihop forwarding...");
        emit packetHop("Node A", "Node B", "Encrypting X25519 + AES-GCM");
        QTimer::singleShot(1500, this, [this]() {
            emit packetHop("Node B", "Node C", "Rolling HMAC verification");
        });
        break;

    case 7: // DELIVERY
        emit logEvent("DELIVERY", "Packet successfully delivered.");
        emit packetDelivered("Node C");
        break;

    case 8: // BLOCKCHAIN STEP
        emit logEvent("BLOCKCHAIN", "Synchronizing ledger state...");
        mineBlock();
        break;

    case 9: // CHAT STEP
        emit logEvent("CHAT", "Initiating P2P communication...");
        sendMessage("Node A", "Node C", "Hello from Node A!");
        break;
    }
}

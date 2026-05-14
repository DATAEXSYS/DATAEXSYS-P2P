#include "backend/AppController.h"
#include <QRandomGenerator>
#include <QDateTime>
#include "../../network/sender.h"
#include "../../network/receiver.h"

AppController::AppController(QObject *parent) : QObject(parent) {
    m_timer = new QTimer(this);
    m_timer->setInterval(2000); // 2 seconds between steps in auto-run
    connect(m_timer, &QTimer::timeout, this, &AppController::nextStep);

    m_trustAdapter = std::make_unique<LocalTrustDiariesAdapter>(this);
    connect(m_trustAdapter.get(),
            &LocalTrustDiariesAdapter::debugLog,
            this,
            [this](const QString &tag, const QString &message) {
                emit logEvent(tag, message);
            });
    connect(m_trustAdapter.get(),
            &LocalTrustDiariesAdapter::errorOccurred,
            this,
            [this](const QString &message) {
                emit logEvent("TRUST", message);
            });

    m_rollingSignaturesAdapter = std::make_unique<RollingSignaturesAdapter>(this);
    connect(m_rollingSignaturesAdapter.get(),
            &RollingSignaturesAdapter::eventStream,
            this,
            [this](const QString &tag, const QString &message) {
                emit logEvent(tag, message);
            });

    m_pkCertChainAdapter = std::make_unique<PKCertChainAdapter>(this);
    connect(m_pkCertChainAdapter.get(),
            &PKCertChainAdapter::chainUpdated,
            this,
            [this](const QString &chainState) {
                emit logEvent("PKCERT", QString("Chain updated: %1").arg(chainState.left(96)));
            });
    connect(m_pkCertChainAdapter.get(),
            &PKCertChainAdapter::errorOccurred,
            this,
            [this](const QString &message) {
                emit logEvent("PKCERT", message);
            });

    m_receiverRunning = true;
    m_receiverThread = std::thread([this]() {
        network::start_receiver([this](const std::string& ip, const std::string& msg) {
            QMetaObject::invokeMethod(this, [this, ip, msg]() {
                QString qMsg = QString::fromStdString(msg);
                QString qIp = QString::fromStdString(ip);
                
                if (qMsg.startsWith("ROUTE:")) {
                    int sep = qMsg.indexOf('|');
                    if (sep != -1) {
                        QString destIp = qMsg.mid(6, sep - 6);
                        QString realMsg = qMsg.mid(sep + 1);
                        
                        if (m_blackholeEnabled.load()) {
                            emit logEvent("ATTACK", QString("BLACKHOLE ATTACK: Dropped packet from %1 destined to %2").arg(qIp, destIp));
                            emit realMessageReceived("Blackhole", QString("Dropped packet destined to %1").arg(destIp));
                            return; // Do not send ACK, do not forward.
                        }

                        emit logEvent("ROUTING", QString("Forwarding message from %1 to %2").arg(qIp, destIp));
                        
                        // Send ACK back to the source
                        QString ackMsg = QString("ACK_ROUTE:%1").arg(destIp);
                        network::send_message(ip, ackMsg.toStdString());
                        
                        // Show in UI that this node is forwarding
                        emit realMessageReceived("Router (" + qIp + ")", QString("Forwarding to %1: %2").arg(destIp, realMsg));

                        // Forward the message
                        QString fwdMsg = QString("FWD_FROM:%1|%2").arg(qIp, realMsg);
                        network::send_message(destIp.toStdString(), fwdMsg.toStdString());
                        return;
                    }
                } else if (qMsg.startsWith("SYBIL:")) {
                    int sep = qMsg.indexOf('|');
                    if (sep != -1) {
                        QString fakeIp = qMsg.mid(6, sep - 6);
                        QString realMsg = qMsg.mid(sep + 1);
                        emit logEvent("ATTACK", QString("SYBIL ATTACK DETECTED: flood packet received from spoofed IP %1").arg(fakeIp));
                        emit realMessageReceived(fakeIp + " (Sybil)", realMsg);
                        return;
                    }
                } else if (qMsg.startsWith("WORMHOLE:")) {
                    int sep = qMsg.indexOf('|');
                    if (sep != -1) {
                        QString fakeIntermediate = qMsg.mid(9, sep - 9);
                        QString realMsg = qMsg.mid(sep + 1);
                        emit logEvent("ATTACK", QString("WORMHOLE ATTACK DETECTED: packet received directly from %1 bypassing %2").arg(qIp, fakeIntermediate));
                        QString displayIp = QString("%1 (WORMHOLE via %2)").arg(qIp, fakeIntermediate);
                        emit realMessageReceived(displayIp, realMsg);
                        return;
                    }
                } else if (qMsg.startsWith("ACK_ROUTE:")) {
                    QString destIp = qMsg.mid(10);
                    emit realMessageReceived("Network ACK", QString("Intermediate router %1 acknowledged packet for %2").arg(qIp, destIp));
                    emit logEvent("ACK", QString("Router %1 acknowledged packet for %2").arg(qIp, destIp));
                    
                    if (m_trustAdapter) {
                        m_trustAdapter->createPeer(qIp);
                        m_trustAdapter->recordInteraction(qIp, "ACK_RECEIVED", true);
                        emit logEvent("TRUST", "On each hop update bayesian trust");
                    }
                    return;
                } else if (qMsg.startsWith("FWD_FROM:")) {
                    int sep = qMsg.indexOf('|');
                    if (sep != -1) {
                        QString origIp = qMsg.mid(9, sep - 9);
                        QString realMsg = qMsg.mid(sep + 1);
                        emit logEvent("RECV_FWD", QString("Message from %1 via router %2: %3").arg(origIp, qIp, realMsg));
                        QString displayIp = QString("%1 (via %2)").arg(origIp, qIp);
                        emit realMessageReceived(displayIp, realMsg);
                        
                        if (m_trustAdapter) {
                            m_trustAdapter->createPeer(qIp);
                            m_trustAdapter->recordInteraction(qIp, "FWD_DELIVERY", true);
                            emit logEvent("TRUST", "On each hop update bayesian trust");
                        }
                        if (m_pkCertChainAdapter) {
                            m_pkCertChainAdapter->createBlock(QString("Recv FWD from %1").arg(origIp));
                            m_pkCertChainAdapter->startMining(1);
                        }
                        return;
                    }
                }
                
                // Normal direct message
                emit logEvent("RECV_DIRECT", QString("Direct message from %1: %2").arg(qIp, qMsg));
                emit realMessageReceived(qIp, qMsg);
                
                if (m_trustAdapter) {
                    m_trustAdapter->createPeer(qIp);
                    m_trustAdapter->recordInteraction(qIp, "DIRECT_DELIVERY", true);
                }
            }, Qt::QueuedConnection);
        }, m_receiverRunning);
    });
}

AppController::~AppController() {
    m_receiverRunning = false;
    if (m_receiverThread.joinable()) {
        m_receiverThread.join();
    }
}

void AppController::setAutoRun(bool autoRun) {
    if (m_autoRun != autoRun) {
        m_autoRun = autoRun;
        if (m_autoRun) m_timer->start();
        else m_timer->stop();
        emit autoRunChanged();
    }
}

void AppController::setBlackholeEnabled(bool enabled) {
    if (m_blackholeEnabled != enabled) {
        m_blackholeEnabled = enabled;
        emit blackholeEnabledChanged();
        
        if (enabled) {
            emit logEvent("SECURITY", "Blackhole attack simulation ENABLED. Node will drop incoming routed packets.");
        } else {
            emit logEvent("SECURITY", "Blackhole attack simulation DISABLED. Normal routing restored.");
        }
    }
}

void AppController::setWormholeEnabled(bool enabled) {
    if (m_wormholeEnabled != enabled) {
        m_wormholeEnabled = enabled;
        emit wormholeEnabledChanged();
        
        if (enabled) {
            emit logEvent("SECURITY", "Wormhole attack simulation ENABLED. Node will bypass intermediate routers.");
        } else {
            emit logEvent("SECURITY", "Wormhole attack simulation DISABLED.");
        }
    }
}

void AppController::setReplayEnabled(bool enabled) {
    if (m_replayEnabled != enabled) {
        m_replayEnabled = enabled;
        emit replayEnabledChanged();
        
        if (enabled) {
            emit logEvent("SECURITY", "Replay attack simulation ENABLED. Node will resend packets continuously.");
        } else {
            emit logEvent("SECURITY", "Replay attack simulation DISABLED.");
        }
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

void AppController::sendRealMessage(const QString &destIp, const QString &text) {
    if (network::send_message(destIp.toStdString(), text.toStdString())) {
        emit logEvent("CHAT_SEND", QString("Direct message to %1: %2").arg(destIp, text));
        emit messageSent("Me", destIp, text);
        
        if (m_pkCertChainAdapter) {
            m_pkCertChainAdapter->createBlock(QString("Direct %1").arg(destIp));
            m_pkCertChainAdapter->startMining(1);
            emit logEvent("BLOCKCHAIN", "Blockchain is added registered to the network now it is able to send data into the network");
        }
        if (m_rollingSignaturesAdapter) {
            m_rollingSignaturesAdapter->createPacket("Me", destIp, text);
            emit logEvent("ROLLING", "Send data into the packet where rolling signatures work and send by destinations");
        }
    } else {
        emit logEvent("CHAT_ERR", QString("Failed to send to %1").arg(destIp));
    }
}

void AppController::sendRoutedMessage(const QString &destIp, const QString &intermediateIp, const QString &text) {
    if (intermediateIp.isEmpty() || intermediateIp.trimmed() == "") {
        sendRealMessage(destIp, text);
        return;
    }
    
    if (m_wormholeEnabled.load()) {
        QString wormholePayload = QString("WORMHOLE:%1|%2").arg(intermediateIp, text);
        if (network::send_message(destIp.toStdString(), wormholePayload.toStdString())) {
            emit logEvent("ATTACK", QString("WORMHOLE: Bypassed %1, sent directly to %2").arg(intermediateIp, destIp));
            emit messageSent("Me", destIp + " (WORMHOLE via " + intermediateIp + ")", text);
        } else {
            emit logEvent("CHAT_ERR", QString("Failed to wormhole to %1").arg(destIp));
        }
        return;
    }

    QString routePayload = QString("ROUTE:%1|%2").arg(destIp, text);
    int timesToSend = m_replayEnabled.load() ? 5 : 1; // Replay attack sends it 5 times

    for (int i = 0; i < timesToSend; ++i) {
        if (network::send_message(intermediateIp.toStdString(), routePayload.toStdString())) {
            if (i == 0) {
                emit logEvent("CHAT_SEND", QString("Sent to %1 via %2: %3").arg(destIp, intermediateIp, text));
                emit messageSent("Me", destIp + " (via " + intermediateIp + ")", text);
                
                if (m_trustAdapter) {
                    m_trustAdapter->createPeer(intermediateIp);
                    m_trustAdapter->recordInteraction(intermediateIp, "PACKET_SENT", true);
                    emit logEvent("TRUST", "On each hop update bayesian trust");
                }
                if (m_pkCertChainAdapter) {
                    m_pkCertChainAdapter->createBlock(QString("Route %1 -> %2").arg(intermediateIp, destIp));
                    m_pkCertChainAdapter->startMining(1);
                    emit logEvent("BLOCKCHAIN", "Blockchain is added registered to the network now it is able to send data into the network");
                }
                if (m_rollingSignaturesAdapter) {
                    m_rollingSignaturesAdapter->createPacket("Me", destIp, text);
                    emit logEvent("ROLLING", "Send data into the packet where rolling signatures work and send by destinations");
                }
            } else {
                emit logEvent("ATTACK", QString("REPLAY ATTACK: Re-sending packet to %1 via %2").arg(destIp, intermediateIp));
            }
        } else {
            if (i == 0) {
                emit logEvent("CHAT_ERR", QString("Failed to route to %1 via %2").arg(destIp, intermediateIp));
            }
        }
    }
}

void AppController::launchSybilAttack(const QString &destIp, const QString &text) {
    if (destIp.isEmpty() || destIp.trimmed() == "") return;
    
    emit logEvent("ATTACK", QString("Launching Sybil Attack on %1").arg(destIp));
    emit realMessageReceived("Sybil Attacker", "Launching flood of 5 spoofed identities...");
    
    for (int i = 0; i < 5; ++i) {
        QString fakeIp = QString("2001:db8::%1:%2")
            .arg(QRandomGenerator::global()->generate() % 9999)
            .arg(QRandomGenerator::global()->generate() % 9999);
            
        QString payload = QString("SYBIL:%1|%2").arg(fakeIp, text.isEmpty() ? "Sybil Spam Packet" : text);
        network::send_message(destIp.toStdString(), payload.toStdString());
    }
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

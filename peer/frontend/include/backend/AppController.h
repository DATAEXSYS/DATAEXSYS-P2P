#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTimer>
#include <QDebug>
#include <memory>
#include <thread>
#include <atomic>

#include "backend/LocalTrustDiariesAdapter.h"
#include "backend/PKCertChainAdapter.h"
#include "backend/RollingSignaturesAdapter.h"

struct Node {
    QString id;
    double trustScore;
    bool certificateIssued;
    QString status;
};

struct Packet {
    QString source;
    QString destination;
    QString currentNode;
    int hopIndex;
    QString stage;
};

struct Block {
    int index;
    QString hash;
    QString prevHash;
    QString timestamp;
};

struct ChatMessage {
    QString from;
    QString to;
    QString text;
    QString status; // "Sent", "Forwarding", "Delivered"
};

class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int executionStep READ executionStep NOTIFY executionStepChanged)
    Q_PROPERTY(bool autoRun READ autoRun WRITE setAutoRun NOTIFY autoRunChanged)
    Q_PROPERTY(QObject* trustAdapter READ trustAdapter CONSTANT)
    Q_PROPERTY(QObject* rollingSignaturesAdapter READ rollingSignaturesAdapter CONSTANT)
    Q_PROPERTY(QObject* pkCertChainAdapter READ pkCertChainAdapter CONSTANT)
    Q_PROPERTY(bool blackholeEnabled READ blackholeEnabled WRITE setBlackholeEnabled NOTIFY blackholeEnabledChanged)

public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController();

    int executionStep() const { return m_executionStep; }
    bool autoRun() const { return m_autoRun; }
    bool blackholeEnabled() const { return m_blackholeEnabled; }
    
    QObject* trustAdapter() const { return m_trustAdapter.get(); }
    QObject* rollingSignaturesAdapter() const { return m_rollingSignaturesAdapter.get(); }
    QObject* pkCertChainAdapter() const { return m_pkCertChainAdapter.get(); }
    
    void setAutoRun(bool autoRun);
    void setBlackholeEnabled(bool enabled);

    Q_INVOKABLE void startEngine();
    Q_INVOKABLE void nextStep();
    Q_INVOKABLE void resetEngine();
    Q_INVOKABLE void mineBlock();
    Q_INVOKABLE void sendMessage(const QString &from, const QString &to, const QString &text);
    Q_INVOKABLE void sendRealMessage(const QString &destIp, const QString &text);
    Q_INVOKABLE void sendRoutedMessage(const QString &destIp, const QString &intermediateIp, const QString &text);

signals:
    void nodeJoined(QString nodeId);
    void certificateIssued(QString nodeId, QString certId);
    void trustUpdated(QString nodeId, double score);
    void routeSelected(QStringList path);
    void packetEntered(QString source, QString destination);
    void packetHop(QString from, QString to, QString stage);
    void packetDelivered(QString nodeId);
    void logEvent(QString tag, QString message);

    void blockMined(int index, QString hash, QString prevHash);
    void messageSent(QString from, QString to, QString text);
    void messageStatusUpdated(QString text, QString status);
    void realMessageReceived(QString ip, QString text);

    void executionStepChanged();
    void autoRunChanged();
    void blackholeEnabledChanged();

private:
    void executeCurrentStep();

    QList<Node> m_nodes;
    QStringList m_route;
    Packet m_activePacket;
    QList<Block> m_blocks;
    int m_executionStep = 0;
    bool m_autoRun = false;
    QTimer *m_timer;
    std::unique_ptr<LocalTrustDiariesAdapter> m_trustAdapter;
    std::unique_ptr<RollingSignaturesAdapter> m_rollingSignaturesAdapter;
    std::unique_ptr<PKCertChainAdapter> m_pkCertChainAdapter;

    std::thread m_receiverThread;
    std::atomic<bool> m_receiverRunning{false};
    std::atomic<bool> m_blackholeEnabled{false};

    const int MAX_STEPS = 9; // Increased steps for blockchain and chat
};

#endif

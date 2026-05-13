#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QTimer>
#include <QDebug>
#include <memory>

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
    Q_PROPERTY(QObject* rollingSignaturesAdapter READ rollingSignaturesAdapter CONSTANT)

public:
    explicit AppController(QObject *parent = nullptr);

    int executionStep() const { return m_executionStep; }
    bool autoRun() const { return m_autoRun; }
    QObject* rollingSignaturesAdapter() const { return m_rollingSignaturesAdapter.get(); }
    void setAutoRun(bool autoRun);

    Q_INVOKABLE void startEngine();
    Q_INVOKABLE void nextStep();
    Q_INVOKABLE void resetEngine();
    Q_INVOKABLE void mineBlock();
    Q_INVOKABLE void sendMessage(const QString &from, const QString &to, const QString &text);

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

    void executionStepChanged();
    void autoRunChanged();

private:
    void executeCurrentStep();

    QList<Node> m_nodes;
    QStringList m_route;
    Packet m_activePacket;
    QList<Block> m_blocks;
    int m_executionStep = 0;
    bool m_autoRun = false;
    QTimer *m_timer;
    std::unique_ptr<RollingSignaturesAdapter> m_rollingSignaturesAdapter;

    const int MAX_STEPS = 9; // Increased steps for blockchain and chat
};

#endif

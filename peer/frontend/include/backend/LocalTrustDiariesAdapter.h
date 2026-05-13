#pragma once

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QString>
#include <QVector>

class LocalTrustDiariesAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString systemState READ systemState NOTIFY stateChanged)
    Q_PROPERTY(int peerCount READ peerCount NOTIFY stateChanged)
    Q_PROPERTY(bool simulationMode READ simulationMode CONSTANT)

public:
    explicit LocalTrustDiariesAdapter(QObject *parent = nullptr);
    ~LocalTrustDiariesAdapter() override = default;

    Q_INVOKABLE void createPeer(const QString &peerId);
    Q_INVOKABLE void deletePeer(const QString &peerId);
    Q_INVOKABLE void setTrust(const QString &peerId, double value);
    Q_INVOKABLE void simulateInteraction(const QString &peerA, const QString &peerB, const QString &type);
    Q_INVOKABLE void resetSystem();
    Q_INVOKABLE void replayLastState();

    Q_INVOKABLE QString peersJson() const;
    Q_INVOKABLE QString timelineJson() const;
    Q_INVOKABLE QString stateSnapshotJson() const;

    QString systemState() const;
    int peerCount() const;
    bool simulationMode() const { return true; }

    // Compatibility aliases for older UI code.
    Q_INVOKABLE void addPeer(const QString &peerId) { createPeer(peerId); }
    Q_INVOKABLE void removePeer(const QString &peerId) { deletePeer(peerId); }
    Q_INVOKABLE void updateTrustScore(const QString &peerId, double value) { setTrust(peerId, value); }
    Q_INVOKABLE void recordInteraction(const QString &peerA, const QString &interactionType, bool success);
    Q_INVOKABLE void clearAllData() { resetSystem(); }
    Q_INVOKABLE QString getAllPeersAsJson() const { return peersJson(); }
    Q_INVOKABLE QString getInteractionHistoryAsJson(const QString &peerId) const;
    Q_INVOKABLE QString getStatusString() const;
    Q_INVOKABLE void testBackendConnection();

signals:
    void peerAdded(QString peerId);
    void peerRemoved(QString peerId);
    void trustUpdated(QString peerId, double value);
    void interactionLogged(QString peerA, QString peerB, QString type);
    void systemReset();
    void errorOccurred(QString message);
    void debugLog(QString tag, QString message);
    void stateChanged();
    void stateUpdated(QString peersJson, QString timelineJson, QString stateJson);

private:
    struct PeerState
    {
        QString peerId;
        double trust = 0.5;
        int interactions = 0;
        int positive = 0;
        int negative = 0;
        QString trend = QStringLiteral("Neutral");
    };

    struct InteractionState
    {
        int index = 0;
        QString peerA;
        QString peerB;
        QString type;
        bool success = false;
        double deltaA = 0.0;
        double deltaB = 0.0;
        QString resultingState;
        QString timestamp;
    };

    mutable QMutex m_mutex;
    QMap<QString, PeerState> m_peers;
    QVector<InteractionState> m_timeline;
    QString m_systemState = QStringLiteral("Simulation lab ready");
    QString m_lastSnapshotJson;

    double clampTrust(double value) const;
    QString trendFor(double trust) const;
    QString interactionTypeKey(const QString &type) const;
    double baseDeltaFor(const QString &type) const;
    bool deterministicOutcome(const QString &peerA, const QString &peerB, const QString &type, int sequence) const;
    quint32 deterministicSeed(const QString &material) const;
    void applyPeerTrustLocked(const QString &peerId, double newTrust, const QString &trendOverride = QString());
    void pushDebugLocked(const QString &tag, const QString &message);
    void emitStateLocked();
    QString buildPeersJsonLocked() const;
    QString buildTimelineJsonLocked() const;
    QString buildStateJsonLocked() const;
    QString buildInteractionHistoryJsonLocked(const QString &peerId) const;
    QString buildStatusStringLocked() const;
    void storeSnapshotLocked();
    void restoreSnapshotLocked(const QString &snapshotJson);
};

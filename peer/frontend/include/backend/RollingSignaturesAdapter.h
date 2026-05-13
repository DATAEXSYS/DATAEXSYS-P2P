#ifndef ROLLINGSIGNATURESADAPTER_H
#define ROLLINGSIGNATURESADAPTER_H

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QDateTime>

class RollingSignaturesAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)
    Q_PROPERTY(bool simulationMode READ simulationMode WRITE setSimulationMode NOTIFY simulationModeChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit RollingSignaturesAdapter(QObject *parent = nullptr);
    ~RollingSignaturesAdapter() override = default;

    Q_INVOKABLE void createPacket(const QString &source, const QString &destination, const QString &payload);
    Q_INVOKABLE void forwardPacket(const QString &packetId);
    Q_INVOKABLE void verifyPacket(const QString &packetId);
    Q_INVOKABLE void injectTamper(const QString &packetId);

    Q_INVOKABLE void runInvalidPacketInjectionTest();
    Q_INVOKABLE QStringList getPacketIds() const;
    Q_INVOKABLE QString getPacketStateJson(const QString &packetId) const;

    bool debugMode() const;
    bool simulationMode() const;
    QString status() const;

public slots:
    void setDebugMode(bool enabled);
    void setSimulationMode(bool enabled);

signals:
    void packetCreated(QString packetId, QString route);
    void packetForwarded(QString packetId, QString fromNode, QString toNode);
    void packetVerified(QString packetId, QString status);
    void tamperDetected(QString packetId, QString reason);
    void errorOccurred(QString message);

    void packetStateUpdated(QString packetId, QString stateJson);
    void eventStream(QString tag, QString message);
    void debugModeChanged(bool enabled);
    void simulationModeChanged(bool enabled);
    void statusChanged(QString status);

private:
    struct PacketState {
        QString packetId;
        QString source;
        QString destination;
        QString originalPayload;
        QString payload;
        QStringList route;
        int currentHop = 0;
        bool tampered = false;
        bool verified = false;
        QString verificationStatus = "PENDING";
        QString lastError;
        QStringList hopSignatures;
        QDateTime createdAt;
        QDateTime updatedAt;
    };

    void createPacketImpl(const QString &source, const QString &destination, const QString &payload);
    void forwardPacketImpl(const QString &packetId);
    void verifyPacketImpl(const QString &packetId);
    void injectTamperImpl(const QString &packetId);

    QString makePacketId();
    QStringList buildRoute(const QString &source, const QString &destination) const;
    QString makeHopSignature(const QString &packetId,
                             const QString &payload,
                             const QString &from,
                             const QString &to,
                             int hopIndex) const;
    QString buildRouteString(const QStringList &route) const;
    QString stateToJson(const PacketState &state) const;
    void emitStateLocked(const PacketState &state);
    void setStatus(const QString &newStatus);

    mutable QMutex m_mutex;
    QHash<QString, PacketState> m_packets;
    quint64 m_sequence = 0;
    bool m_debugMode = true;
    bool m_simulationMode = true;
    bool m_backendAvailable = false;
    QString m_status;
};

#endif

#include "backend/RollingSignaturesAdapter.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QRandomGenerator>

RollingSignaturesAdapter::RollingSignaturesAdapter(QObject *parent)
    : QObject(parent)
{
    setStatus("Simulation mode active");
    emit eventStream("SYSTEM", "RollingSignatures adapter initialized in isolated simulation mode");
}

void RollingSignaturesAdapter::createPacket(const QString &source,
                                            const QString &destination,
                                            const QString &payload)
{
    QMetaObject::invokeMethod(this, [this, source, destination, payload]() {
        createPacketImpl(source, destination, payload);
    }, Qt::QueuedConnection);
}

void RollingSignaturesAdapter::forwardPacket(const QString &packetId)
{
    QMetaObject::invokeMethod(this, [this, packetId]() {
        forwardPacketImpl(packetId.trimmed());
    }, Qt::QueuedConnection);
}

void RollingSignaturesAdapter::verifyPacket(const QString &packetId)
{
    QMetaObject::invokeMethod(this, [this, packetId]() {
        verifyPacketImpl(packetId.trimmed());
    }, Qt::QueuedConnection);
}

void RollingSignaturesAdapter::injectTamper(const QString &packetId)
{
    QMetaObject::invokeMethod(this, [this, packetId]() {
        injectTamperImpl(packetId.trimmed());
    }, Qt::QueuedConnection);
}

void RollingSignaturesAdapter::runInvalidPacketInjectionTest()
{
    QMetaObject::invokeMethod(this, [this]() {
        const QString invalidId = QStringLiteral("pkt_invalid_%1")
                                      .arg(QRandomGenerator::global()->bounded(100000));
        emit eventStream("TEST", QString("Injecting invalid packet test: %1").arg(invalidId));
        forwardPacketImpl(invalidId);
        verifyPacketImpl(invalidId);
    }, Qt::QueuedConnection);
}

QStringList RollingSignaturesAdapter::getPacketIds() const
{
    QMutexLocker lock(&m_mutex);
    return m_packets.keys();
}

QString RollingSignaturesAdapter::getPacketStateJson(const QString &packetId) const
{
    QMutexLocker lock(&m_mutex);
    const QString key = packetId.trimmed();
    if (!m_packets.contains(key)) {
        return QStringLiteral("{}");
    }
    return stateToJson(m_packets.value(key));
}

bool RollingSignaturesAdapter::debugMode() const
{
    QMutexLocker lock(&m_mutex);
    return m_debugMode;
}

bool RollingSignaturesAdapter::simulationMode() const
{
    QMutexLocker lock(&m_mutex);
    return m_simulationMode;
}

QString RollingSignaturesAdapter::status() const
{
    QMutexLocker lock(&m_mutex);
    return m_status;
}

void RollingSignaturesAdapter::setDebugMode(bool enabled)
{
    bool changed = false;
    {
        QMutexLocker lock(&m_mutex);
        if (m_debugMode != enabled) {
            m_debugMode = enabled;
            changed = true;
        }
    }

    if (changed) {
        emit debugModeChanged(enabled);
        emit eventStream("CONFIG", QString("Debug mode set to %1").arg(enabled ? "ON" : "OFF"));
    }
}

void RollingSignaturesAdapter::setSimulationMode(bool enabled)
{
    bool changed = false;
    {
        QMutexLocker lock(&m_mutex);
        if (m_simulationMode != enabled) {
            m_simulationMode = enabled;
            changed = true;
        }
    }

    if (changed) {
        setStatus(enabled ? "Simulation mode active" : "Backend mode requested");
        emit simulationModeChanged(enabled);
        emit eventStream("CONFIG", QString("Simulation mode set to %1").arg(enabled ? "ON" : "OFF"));
    }
}

void RollingSignaturesAdapter::createPacketImpl(const QString &source,
                                                const QString &destination,
                                                const QString &payload)
{
    const QString src = source.trimmed();
    const QString dst = destination.trimmed();

    if (src.isEmpty() || dst.isEmpty()) {
        emit errorOccurred("Source and destination are required");
        emit eventStream("ERROR", "Cannot create packet: source/destination missing");
        return;
    }

    if (payload.isEmpty()) {
        emit errorOccurred("Payload cannot be empty");
        emit eventStream("ERROR", "Cannot create packet: payload missing");
        return;
    }

    PacketState state;
    state.packetId = makePacketId();
    state.source = src;
    state.destination = dst;
    state.originalPayload = payload;
    state.payload = payload;
    state.route = buildRoute(src, dst);
    state.currentHop = 0;
    state.createdAt = QDateTime::currentDateTimeUtc();
    state.updatedAt = state.createdAt;

    for (int i = 0; i < state.route.size() - 1; ++i) {
        state.hopSignatures.append(makeHopSignature(state.packetId,
                                                    state.payload,
                                                    state.route[i],
                                                    state.route[i + 1],
                                                    i));
    }

    {
        QMutexLocker lock(&m_mutex);
        m_packets.insert(state.packetId, state);
    }

    emit packetCreated(state.packetId, buildRouteString(state.route));
    emit eventStream("CREATE", QString("Created packet %1 (%2 -> %3)")
                                   .arg(state.packetId, state.source, state.destination));
    emit packetStateUpdated(state.packetId, stateToJson(state));
}

void RollingSignaturesAdapter::forwardPacketImpl(const QString &packetId)
{
    PacketState state;
    QString fromNode;
    QString toNode;

    {
        QMutexLocker lock(&m_mutex);
        if (!m_packets.contains(packetId)) {
            emit errorOccurred(QString("Packet not found: %1").arg(packetId));
            emit eventStream("ERROR", QString("Forward failed: unknown packet %1").arg(packetId));
            return;
        }

        state = m_packets.value(packetId);
        if (state.route.size() < 2) {
            state.lastError = "Invalid route";
            m_packets[packetId] = state;
            emit errorOccurred(QString("Invalid route for packet %1").arg(packetId));
            emit eventStream("ERROR", QString("Forward failed: invalid route for %1").arg(packetId));
            emitStateLocked(state);
            return;
        }

        if (state.currentHop >= state.route.size() - 1) {
            emit eventStream("FORWARD", QString("Packet %1 already at destination").arg(packetId));
            emit packetVerified(packetId, state.verified ? state.verificationStatus : "AT_DESTINATION_UNVERIFIED");
            emitStateLocked(state);
            return;
        }

        fromNode = state.route[state.currentHop];
        toNode = state.route[state.currentHop + 1];
        state.currentHop += 1;
        state.updatedAt = QDateTime::currentDateTimeUtc();

        m_packets[packetId] = state;
    }

    emit packetForwarded(packetId, fromNode, toNode);
    emit eventStream("FORWARD", QString("Packet %1 moved %2 -> %3").arg(packetId, fromNode, toNode));
    emit packetStateUpdated(packetId, stateToJson(state));
}

void RollingSignaturesAdapter::verifyPacketImpl(const QString &packetId)
{
    PacketState state;
    QString verifyStatus;

    {
        QMutexLocker lock(&m_mutex);
        if (!m_packets.contains(packetId)) {
            emit errorOccurred(QString("Packet not found: %1").arg(packetId));
            emit eventStream("ERROR", QString("Verify failed: unknown packet %1").arg(packetId));
            return;
        }

        state = m_packets.value(packetId);

        if (state.currentHop <= 0) {
            verifyStatus = "NOT_FORWARDED";
            state.verified = false;
            state.verificationStatus = verifyStatus;
        } else {
            const int signatureIndex = state.currentHop - 1;
            const QString expected = state.hopSignatures.value(signatureIndex);
            const QString actual = makeHopSignature(state.packetId,
                                                    state.payload,
                                                    state.route[signatureIndex],
                                                    state.route[signatureIndex + 1],
                                                    signatureIndex);

            if (state.tampered || expected != actual) {
                verifyStatus = "TAMPERED";
                state.verified = false;
                state.verificationStatus = verifyStatus;
                emit tamperDetected(packetId, QString("HMAC mismatch at hop %1").arg(signatureIndex));
                emit eventStream("ALERT", QString("Tamper detected for %1 at hop %2")
                                          .arg(packetId)
                                          .arg(signatureIndex));
            } else if (state.currentHop >= state.route.size() - 1) {
                verifyStatus = "VERIFIED_DESTINATION";
                state.verified = true;
                state.verificationStatus = verifyStatus;
            } else {
                verifyStatus = "VERIFIED_IN_TRANSIT";
                state.verified = true;
                state.verificationStatus = verifyStatus;
            }
        }

        state.updatedAt = QDateTime::currentDateTimeUtc();
        m_packets[packetId] = state;
    }

    emit packetVerified(packetId, verifyStatus);
    emit eventStream("VERIFY", QString("Packet %1 verification: %2").arg(packetId, verifyStatus));
    emit packetStateUpdated(packetId, stateToJson(state));
}

void RollingSignaturesAdapter::injectTamperImpl(const QString &packetId)
{
    PacketState state;

    {
        QMutexLocker lock(&m_mutex);
        if (!m_packets.contains(packetId)) {
            emit errorOccurred(QString("Packet not found: %1").arg(packetId));
            emit eventStream("ERROR", QString("Tamper injection failed: unknown packet %1").arg(packetId));
            return;
        }

        if (!m_debugMode) {
            emit errorOccurred("Tamper injection is available only in debug mode");
            emit eventStream("WARN", "Tamper injection blocked (debug mode OFF)");
            return;
        }

        state = m_packets.value(packetId);
        state.tampered = true;
        state.payload = QStringLiteral("TAMPERED::%1").arg(state.payload);
        state.updatedAt = QDateTime::currentDateTimeUtc();
        m_packets[packetId] = state;
    }

    emit tamperDetected(packetId, "Manual tamper injection requested");
    emit eventStream("DEBUG", QString("Tamper injected into packet %1").arg(packetId));
    emit packetStateUpdated(packetId, stateToJson(state));
}

QString RollingSignaturesAdapter::makePacketId()
{
    QMutexLocker lock(&m_mutex);
    m_sequence += 1;
    return QStringLiteral("pkt_%1_%2")
        .arg(QDateTime::currentMSecsSinceEpoch())
        .arg(m_sequence);
}

QStringList RollingSignaturesAdapter::buildRoute(const QString &source, const QString &destination) const
{
    const QStringList relays = {"Relay-B", "Relay-C", "Relay-D", "Relay-E"};
    const int hopCount = 2 + (QRandomGenerator::global()->bounded(2));

    QStringList route;
    route << source;
    for (int i = 0; i < hopCount; ++i) {
        route << relays[(QRandomGenerator::global()->bounded(relays.size()) + i) % relays.size()];
    }
    route << destination;

    return route;
}

QString RollingSignaturesAdapter::makeHopSignature(const QString &packetId,
                                                   const QString &payload,
                                                   const QString &from,
                                                   const QString &to,
                                                   int hopIndex) const
{
    const QString material = QStringLiteral("%1|%2|%3|%4|%5")
                                 .arg(packetId, payload, from, to)
                                 .arg(hopIndex);

    const QByteArray digest = QCryptographicHash::hash(material.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.toHex().left(16));
}

QString RollingSignaturesAdapter::buildRouteString(const QStringList &route) const
{
    return route.join(" -> ");
}

QString RollingSignaturesAdapter::stateToJson(const PacketState &state) const
{
    QJsonObject obj;
    obj["packetId"] = state.packetId;
    obj["source"] = state.source;
    obj["destination"] = state.destination;
    obj["payload"] = state.payload;
    obj["currentHop"] = state.currentHop;
    obj["hopCount"] = state.route.size() > 1 ? (state.route.size() - 1) : 0;
    obj["tampered"] = state.tampered;
    obj["verified"] = state.verified;
    obj["verificationStatus"] = state.verificationStatus;
    obj["createdAt"] = state.createdAt.toString(Qt::ISODate);
    obj["updatedAt"] = state.updatedAt.toString(Qt::ISODate);

    QJsonArray routeArray;
    for (const QString &node : state.route) {
        routeArray.append(node);
    }
    obj["route"] = routeArray;

    QJsonArray signatures;
    for (const QString &sig : state.hopSignatures) {
        signatures.append(sig);
    }
    obj["hopSignatures"] = signatures;

    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void RollingSignaturesAdapter::emitStateLocked(const PacketState &state)
{
    emit packetStateUpdated(state.packetId, stateToJson(state));
}

void RollingSignaturesAdapter::setStatus(const QString &newStatus)
{
    bool changed = false;
    {
        QMutexLocker lock(&m_mutex);
        if (m_status != newStatus) {
            m_status = newStatus;
            changed = true;
        }
    }

    if (changed) {
        emit statusChanged(newStatus);
    }
}

#include "backend/LocalTrustDiariesAdapter.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

namespace {
static QString jsonCompact(const QJsonObject &object)
{
    return QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

static QString jsonCompact(const QJsonArray &array)
{
    return QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact));
}
}

LocalTrustDiariesAdapter::LocalTrustDiariesAdapter(QObject *parent)
    : QObject(parent)
{
    m_systemState = QStringLiteral("Simulation lab ready");
    emit debugLog(QStringLiteral("SYSTEM"), QStringLiteral("LocalTrustDiaries simulation engine initialized"));
    emitStateLocked();
}

void LocalTrustDiariesAdapter::createPeer(const QString &peerId)
{
    const QString id = peerId.trimmed();
    if (id.isEmpty()) {
        emit errorOccurred(QStringLiteral("Peer ID cannot be empty"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (m_peers.contains(id)) {
        emit errorOccurred(QStringLiteral("Peer already exists: %1").arg(id));
        return;
    }

    PeerState state;
    state.peerId = id;
    state.trust = 0.5;
    state.trend = QStringLiteral("Neutral");
    m_peers.insert(id, state);
    m_systemState = QStringLiteral("Peer created: %1").arg(id);

    pushDebugLocked(QStringLiteral("PEER"), QStringLiteral("Created peer %1 at trust 0.50").arg(id));
    emit peerAdded(id);
    emitStateLocked();
}

void LocalTrustDiariesAdapter::deletePeer(const QString &peerId)
{
    const QString id = peerId.trimmed();
    if (id.isEmpty()) {
        emit errorOccurred(QStringLiteral("Peer ID cannot be empty"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (!m_peers.contains(id)) {
        emit errorOccurred(QStringLiteral("Peer not found: %1").arg(id));
        return;
    }

    m_peers.remove(id);
    m_systemState = QStringLiteral("Peer removed: %1").arg(id);
    pushDebugLocked(QStringLiteral("PEER"), QStringLiteral("Removed peer %1").arg(id));
    emit peerRemoved(id);
    emitStateLocked();
}

void LocalTrustDiariesAdapter::setTrust(const QString &peerId, double value)
{
    const QString id = peerId.trimmed();
    if (id.isEmpty()) {
        emit errorOccurred(QStringLiteral("Peer ID cannot be empty"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (!m_peers.contains(id)) {
        emit errorOccurred(QStringLiteral("Peer not found: %1").arg(id));
        return;
    }

    const double clamped = clampTrust(value);
    applyPeerTrustLocked(id, clamped);
    m_systemState = QStringLiteral("Trust updated: %1").arg(id);
    pushDebugLocked(QStringLiteral("TRUST"), QStringLiteral("Set trust for %1 to %2").arg(id).arg(clamped, 0, 'f', 2));
    emit trustUpdated(id, clamped);
    emitStateLocked();
}

void LocalTrustDiariesAdapter::simulateInteraction(const QString &peerA, const QString &peerB, const QString &type)
{
    const QString left = peerA.trimmed();
    const QString right = peerB.trimmed();
    const QString interactionType = interactionTypeKey(type);

    if (left.isEmpty() || right.isEmpty()) {
        emit errorOccurred(QStringLiteral("Both peer IDs are required"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (!m_peers.contains(left)) {
        emit errorOccurred(QStringLiteral("Peer not found: %1").arg(left));
        return;
    }
    if (!m_peers.contains(right)) {
        emit errorOccurred(QStringLiteral("Peer not found: %1").arg(right));
        return;
    }

    const int sequence = m_timeline.size();
    const bool success = deterministicOutcome(left, right, interactionType, sequence);
    const double baseDelta = baseDeltaFor(interactionType);
    const double deltaA = success ? baseDelta : -baseDelta * 1.5;
    const double deltaB = success ? baseDelta * 0.6 : -baseDelta * 0.9;

    PeerState leftState = m_peers.value(left);
    PeerState rightState = m_peers.value(right);

    leftState.interactions += 1;
    rightState.interactions += 1;
    if (success) {
        leftState.positive += 1;
        rightState.positive += 1;
    } else {
        leftState.negative += 1;
        rightState.negative += 1;
    }

    applyPeerTrustLocked(left, clampTrust(leftState.trust + deltaA));
    applyPeerTrustLocked(right, clampTrust(rightState.trust + deltaB));

    InteractionState interaction;
    interaction.index = sequence;
    interaction.peerA = left;
    interaction.peerB = right;
    interaction.type = interactionType;
    interaction.success = success;
    interaction.deltaA = deltaA;
    interaction.deltaB = deltaB;
    interaction.resultingState = QStringLiteral("%1=%2 | %3=%4")
                                    .arg(left)
                                    .arg(m_peers.value(left).trust, 0, 'f', 2)
                                    .arg(right)
                                    .arg(m_peers.value(right).trust, 0, 'f', 2);
    interaction.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    m_timeline.append(interaction);
    m_systemState = QStringLiteral("Interaction logged: %1 -> %2 (%3)").arg(left, right, interactionType);

    pushDebugLocked(QStringLiteral("INTERACTION"),
                    QStringLiteral("%1 / %2 / %3 => %4")
                        .arg(left, right, interactionType, success ? QStringLiteral("SUCCESS") : QStringLiteral("FAILURE")));

    emit trustUpdated(left, m_peers.value(left).trust);
    emit trustUpdated(right, m_peers.value(right).trust);
    emit interactionLogged(left, right, interactionType);
    emitStateLocked();
}

void LocalTrustDiariesAdapter::resetSystem()
{
    QMutexLocker locker(&m_mutex);
    storeSnapshotLocked();
    m_peers.clear();
    m_timeline.clear();
    m_systemState = QStringLiteral("System reset");

    pushDebugLocked(QStringLiteral("SYSTEM"), QStringLiteral("System reset requested"));
    emit systemReset();
    emitStateLocked();
}

void LocalTrustDiariesAdapter::replayLastState()
{
    QMutexLocker locker(&m_mutex);
    if (m_lastSnapshotJson.isEmpty()) {
        emit errorOccurred(QStringLiteral("No snapshot available to replay"));
        return;
    }

    restoreSnapshotLocked(m_lastSnapshotJson);
    m_systemState = QStringLiteral("Snapshot replayed");
    pushDebugLocked(QStringLiteral("SYSTEM"), QStringLiteral("Replayed last saved state"));
    emitStateLocked();
}

QString LocalTrustDiariesAdapter::peersJson() const
{
    QMutexLocker locker(&m_mutex);
    return buildPeersJsonLocked();
}

QString LocalTrustDiariesAdapter::timelineJson() const
{
    QMutexLocker locker(&m_mutex);
    return buildTimelineJsonLocked();
}

QString LocalTrustDiariesAdapter::stateSnapshotJson() const
{
    QMutexLocker locker(&m_mutex);
    return buildStateJsonLocked();
}

QString LocalTrustDiariesAdapter::systemState() const
{
    QMutexLocker locker(&m_mutex);
    return m_systemState;
}

int LocalTrustDiariesAdapter::peerCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_peers.size();
}

void LocalTrustDiariesAdapter::recordInteraction(const QString &peerA, const QString &interactionType, bool success)
{
    const QString id = peerA.trimmed();
    if (id.isEmpty()) {
        emit errorOccurred(QStringLiteral("Peer ID cannot be empty"));
        return;
    }

    QMutexLocker locker(&m_mutex);
    if (!m_peers.contains(id)) {
        emit errorOccurred(QStringLiteral("Peer not found: %1").arg(id));
        return;
    }

    const QString normalizedType = interactionTypeKey(interactionType);
    PeerState state = m_peers.value(id);
    state.interactions += 1;
    if (success) {
        state.positive += 1;
    } else {
        state.negative += 1;
    }

    const double delta = success ? baseDeltaFor(normalizedType) : -baseDeltaFor(normalizedType) * 1.5;
    applyPeerTrustLocked(id, clampTrust(state.trust + delta));

    InteractionState interaction;
    interaction.index = m_timeline.size();
    interaction.peerA = id;
    interaction.peerB = id;
    interaction.type = normalizedType;
    interaction.success = success;
    interaction.deltaA = delta;
    interaction.deltaB = 0.0;
    interaction.resultingState = QStringLiteral("%1=%2")
                                     .arg(id)
                                     .arg(m_peers.value(id).trust, 0, 'f', 2);
    interaction.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    m_timeline.append(interaction);
    m_systemState = QStringLiteral("Interaction recorded: %1").arg(id);

    pushDebugLocked(QStringLiteral("INTERACTION"),
                    QStringLiteral("%1 / %2 => %3")
                        .arg(id, normalizedType, success ? QStringLiteral("SUCCESS") : QStringLiteral("FAILURE")));

    emit trustUpdated(id, m_peers.value(id).trust);
    emit interactionLogged(id, id, normalizedType);
    emitStateLocked();
}

QString LocalTrustDiariesAdapter::getInteractionHistoryAsJson(const QString &peerId) const
{
    QMutexLocker locker(&m_mutex);
    return buildInteractionHistoryJsonLocked(peerId.trimmed());
}

QString LocalTrustDiariesAdapter::getStatusString() const
{
    QMutexLocker locker(&m_mutex);
    return buildStatusStringLocked();
}

void LocalTrustDiariesAdapter::testBackendConnection()
{
    QMutexLocker locker(&m_mutex);
    pushDebugLocked(QStringLiteral("TEST"), QStringLiteral("Simulation mode active - no backend required"));
    emit debugLog(QStringLiteral("TEST"), QStringLiteral("Simulation mode active - no backend required"));
    emitStateLocked();
}

double LocalTrustDiariesAdapter::clampTrust(double value) const
{
    if (value < 0.0) {
        return 0.0;
    }
    if (value > 1.0) {
        return 1.0;
    }
    return value;
}

QString LocalTrustDiariesAdapter::trendFor(double trust) const
{
    if (trust >= 0.7) {
        return QStringLiteral("Rising");
    }
    if (trust <= 0.3) {
        return QStringLiteral("Declining");
    }
    return QStringLiteral("Stable");
}

QString LocalTrustDiariesAdapter::interactionTypeKey(const QString &type) const
{
    const QString key = type.trimmed().toLower();
    if (key.isEmpty()) {
        return QStringLiteral("generic");
    }
    return key;
}

double LocalTrustDiariesAdapter::baseDeltaFor(const QString &type) const
{
    if (type == QStringLiteral("send")) return 0.03;
    if (type == QStringLiteral("receive")) return 0.02;
    if (type == QStringLiteral("validate")) return 0.05;
    if (type == QStringLiteral("forward")) return 0.015;
    if (type == QStringLiteral("store")) return 0.01;
    return 0.025;
}

quint32 LocalTrustDiariesAdapter::deterministicSeed(const QString &material) const
{
    const QByteArray digest = QCryptographicHash::hash(material.toUtf8(), QCryptographicHash::Sha256);
    quint32 seed = 0;
    for (int i = 0; i < 4 && i < digest.size(); ++i) {
        seed = (seed << 8) | static_cast<quint8>(digest.at(i));
    }
    return seed;
}

bool LocalTrustDiariesAdapter::deterministicOutcome(const QString &peerA,
                                                    const QString &peerB,
                                                    const QString &type,
                                                    int sequence) const
{
    const quint32 seed = deterministicSeed(peerA + QStringLiteral("|") + peerB + QStringLiteral("|") + type + QStringLiteral("|") + QString::number(sequence));
    const int score = static_cast<int>(seed % 100);

    const PeerState left = m_peers.value(peerA);
    const PeerState right = m_peers.value(peerB);
    const int trustScore = qRound(((left.trust + right.trust) / 2.0) * 100.0);

    return score <= trustScore;
}

void LocalTrustDiariesAdapter::applyPeerTrustLocked(const QString &peerId,
                                                    double newTrust,
                                                    const QString &trendOverride)
{
    if (!m_peers.contains(peerId)) {
        return;
    }

    PeerState state = m_peers.value(peerId);
    state.trust = clampTrust(newTrust);
    state.trend = trendOverride.isEmpty() ? trendFor(state.trust) : trendOverride;
    m_peers.insert(peerId, state);
}

void LocalTrustDiariesAdapter::pushDebugLocked(const QString &tag, const QString &message)
{
    emit debugLog(tag, message);
}

void LocalTrustDiariesAdapter::emitStateLocked()
{
    const QString peers = buildPeersJsonLocked();
    const QString timeline = buildTimelineJsonLocked();
    const QString state = buildStateJsonLocked();
    m_lastSnapshotJson = state;
    emit stateChanged();
    emit stateUpdated(peers, timeline, state);
}

QString LocalTrustDiariesAdapter::buildPeersJsonLocked() const
{
    QJsonArray peers;
    for (const auto &peer : m_peers) {
        QJsonObject object;
        object[QStringLiteral("peerId")] = peer.peerId;
        object[QStringLiteral("trust")] = peer.trust;
        object[QStringLiteral("interactions")] = peer.interactions;
        object[QStringLiteral("positive")] = peer.positive;
        object[QStringLiteral("negative")] = peer.negative;
        object[QStringLiteral("trend")] = peer.trend;
        peers.append(object);
    }
    return jsonCompact(peers);
}

QString LocalTrustDiariesAdapter::buildTimelineJsonLocked() const
{
    QJsonArray timeline;
    for (const auto &interaction : m_timeline) {
        QJsonObject object;
        object[QStringLiteral("index")] = interaction.index;
        object[QStringLiteral("peerA")] = interaction.peerA;
        object[QStringLiteral("peerB")] = interaction.peerB;
        object[QStringLiteral("type")] = interaction.type;
        object[QStringLiteral("success")] = interaction.success;
        object[QStringLiteral("deltaA")] = interaction.deltaA;
        object[QStringLiteral("deltaB")] = interaction.deltaB;
        object[QStringLiteral("resultingState")] = interaction.resultingState;
        object[QStringLiteral("timestamp")] = interaction.timestamp;
        timeline.append(object);
    }
    return jsonCompact(timeline);
}

QString LocalTrustDiariesAdapter::buildStateJsonLocked() const
{
    QJsonObject state;
    state[QStringLiteral("systemState")] = m_systemState;
    state[QStringLiteral("peerCount")] = static_cast<int>(m_peers.size());
    state[QStringLiteral("peers")] = QJsonDocument::fromJson(buildPeersJsonLocked().toUtf8()).array();
    state[QStringLiteral("timeline")] = QJsonDocument::fromJson(buildTimelineJsonLocked().toUtf8()).array();
    state[QStringLiteral("timestamp")] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    return jsonCompact(state);
}

QString LocalTrustDiariesAdapter::buildInteractionHistoryJsonLocked(const QString &peerId) const
{
    QJsonArray history;
    for (const auto &interaction : m_timeline) {
        if (interaction.peerA != peerId && interaction.peerB != peerId) {
            continue;
        }

        QJsonObject object;
        object[QStringLiteral("index")] = interaction.index;
        object[QStringLiteral("peerA")] = interaction.peerA;
        object[QStringLiteral("peerB")] = interaction.peerB;
        object[QStringLiteral("type")] = interaction.type;
        object[QStringLiteral("success")] = interaction.success;
        object[QStringLiteral("timestamp")] = interaction.timestamp;
        history.append(object);
    }
    return jsonCompact(history);
}

QString LocalTrustDiariesAdapter::buildStatusStringLocked() const
{
    return QStringLiteral("Simulation ready | %1 peers | %2 interactions | %3")
        .arg(m_peers.size())
        .arg(m_timeline.size())
        .arg(m_systemState);
}

void LocalTrustDiariesAdapter::storeSnapshotLocked()
{
    m_lastSnapshotJson = buildStateJsonLocked();
}

void LocalTrustDiariesAdapter::restoreSnapshotLocked(const QString &snapshotJson)
{
    const QJsonDocument document = QJsonDocument::fromJson(snapshotJson.toUtf8());
    if (!document.isObject()) {
        return;
    }

    const QJsonObject state = document.object();
    m_peers.clear();
    m_timeline.clear();

    m_systemState = state.value(QStringLiteral("systemState")).toString(QStringLiteral("Simulation lab ready"));

    const QJsonArray peers = state.value(QStringLiteral("peers")).toArray();
    for (const auto &value : peers) {
        const QJsonObject object = value.toObject();
        PeerState peer;
        peer.peerId = object.value(QStringLiteral("peerId")).toString();
        peer.trust = object.value(QStringLiteral("trust")).toDouble(0.5);
        peer.interactions = object.value(QStringLiteral("interactions")).toInt(0);
        peer.positive = object.value(QStringLiteral("positive")).toInt(0);
        peer.negative = object.value(QStringLiteral("negative")).toInt(0);
        peer.trend = object.value(QStringLiteral("trend")).toString(QStringLiteral("Neutral"));
        if (!peer.peerId.isEmpty()) {
            m_peers.insert(peer.peerId, peer);
        }
    }

    const QJsonArray timeline = state.value(QStringLiteral("timeline")).toArray();
    for (const auto &value : timeline) {
        const QJsonObject object = value.toObject();
        InteractionState interaction;
        interaction.index = object.value(QStringLiteral("index")).toInt(0);
        interaction.peerA = object.value(QStringLiteral("peerA")).toString();
        interaction.peerB = object.value(QStringLiteral("peerB")).toString();
        interaction.type = object.value(QStringLiteral("type")).toString();
        interaction.success = object.value(QStringLiteral("success")).toBool(false);
        interaction.deltaA = object.value(QStringLiteral("deltaA")).toDouble(0.0);
        interaction.deltaB = object.value(QStringLiteral("deltaB")).toDouble(0.0);
        interaction.resultingState = object.value(QStringLiteral("resultingState")).toString();
        interaction.timestamp = object.value(QStringLiteral("timestamp")).toString();
        m_timeline.append(interaction);
    }
}

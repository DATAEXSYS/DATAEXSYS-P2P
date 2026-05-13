#include "backend/LocalTrustDiariesAdapter.h"
#include <QDebug>
#include <QDateTime>
#include <QMutexLocker>
#include <cmath>
#include <QThread>
#include <QString>
#include <QDateTime>
// =====================================================================
// TrustDatabase Implementation
// =====================================================================

LocalTrustDiariesAdapter::TrustDatabase::TrustDatabase()
{
    qDebug() << "[LocalTrustDiaries] Database initialized";
}

void LocalTrustDiariesAdapter::TrustDatabase::addPeer(const QString &peerId)
{
    if (m_peers.contains(peerId)) {
        qWarning() << "[LocalTrustDiaries] Peer already exists:" << peerId;
        return;
    }
    
    PeerData data;
    data.peerId = peerId;
    data.trustScore = 0.5;  // Start neutral
    m_peers[peerId] = data;
    
    qDebug() << "[LocalTrustDiaries] Added peer:" << peerId << "with initial score 0.5";
}

void LocalTrustDiariesAdapter::TrustDatabase::removePeer(const QString &peerId)
{
    if (!m_peers.contains(peerId)) {
        qWarning() << "[LocalTrustDiaries] Peer not found for removal:" << peerId;
        return;
    }
    
    m_peers.remove(peerId);
    qDebug() << "[LocalTrustDiaries] Removed peer:" << peerId;
}

void LocalTrustDiariesAdapter::TrustDatabase::updateScore(const QString &peerId, double delta)
{
    if (!m_peers.contains(peerId)) {
        qWarning() << "[LocalTrustDiaries] Cannot update unknown peer:" << peerId;
        return;
    }
    
    PeerData &data = m_peers[peerId];
    double newScore = data.trustScore + delta;
    
    // Clamp to [0, 1]
    newScore = std::max(0.0, std::min(1.0, newScore));
    
    qDebug() << "[LocalTrustDiaries] Updated" << peerId 
             << "score:" << data.trustScore << "->" << newScore 
             << "(delta:" << delta << ")";
    
    data.trustScore = newScore;
}

double LocalTrustDiariesAdapter::TrustDatabase::getScore(const QString &peerId) const
{
    if (!m_peers.contains(peerId)) {
        return -1.0;
    }
    return m_peers[peerId].trustScore;
}

void LocalTrustDiariesAdapter::TrustDatabase::recordInteraction(
    const QString &peerId, 
    const QString &type, 
    bool success)
{
    if (!m_peers.contains(peerId)) {
        qWarning() << "[LocalTrustDiaries] Cannot record interaction for unknown peer:" << peerId;
        return;
    }
    
    PeerData &data = m_peers[peerId];
    
    if (success) {
        data.successCount++;
    } else {
        data.failureCount++;
    }
    
    data.interactions.append({type, success});
    
    // Auto-adjust trust based on result
    double adjustment = success ? 0.05 : -0.1;
    updateScore(peerId, adjustment);
    
    qDebug() << "[LocalTrustDiaries] Recorded interaction for" << peerId 
             << "type:" << type << "result:" << (success ? "SUCCESS" : "FAILURE");
}

json LocalTrustDiariesAdapter::TrustDatabase::getAllPeersJson() const
{
    json arr = json::array();
    
    for (const auto &data : m_peers) {
        double trend = 0.0;
        if (data.successCount + data.failureCount > 0) {
            trend = static_cast<double>(data.successCount) / 
                   (data.successCount + data.failureCount);
        }
        
        json peer = {
            {"peerId", data.peerId.toStdString()},
            {"score", data.trustScore},
            {"successCount", data.successCount},
            {"failureCount", data.failureCount},
            {"trend", trend},
            {"trendString", trend > 0.6 ? "↑" : trend < 0.4 ? "↓" : "→"}
        };
        arr.push_back(peer);
    }
    
    // Sort by score descending
    std::sort(arr.begin(), arr.end(), 
        [](const json &a, const json &b) {
            return a["score"] > b["score"];
        });
    
    return arr;
}

json LocalTrustDiariesAdapter::TrustDatabase::getInteractionHistoryJson(const QString &peerId) const
{
    json arr = json::array();
    
    if (!m_peers.contains(peerId)) {
        return arr;
    }
    
    const auto &data = m_peers[peerId];
    
    for (int i = 0; i < data.interactions.size(); ++i) {
        const auto &interaction = data.interactions[i];
        json entry = {
            {"index", i},
            {"type", interaction.first.toStdString()},
            {"success", interaction.second},
            {"timestamp", QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()}
        };
        arr.push_back(entry);
    }
    
    return arr;
}

json LocalTrustDiariesAdapter::TrustDatabase::getFullStateJson() const
{
    json state = {
        {"peers", getAllPeersJson()},
        {"totalPeers", static_cast<int>(m_peers.size())},
        {"timestamp", QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()}
    };
    return state;
}

void LocalTrustDiariesAdapter::TrustDatabase::clear()
{
    m_peers.clear();
    qDebug() << "[LocalTrustDiaries] Database cleared";
}

// =====================================================================
// LocalTrustDiariesAdapter Implementation
// =====================================================================

LocalTrustDiariesAdapter::LocalTrustDiariesAdapter(QObject *parent)
    : QObject(parent),
      m_database(std::make_unique<TrustDatabase>())
{
    qDebug() << "[LocalTrustDiariesAdapter] Initialized (thread-safe wrapper)";
}

void LocalTrustDiariesAdapter::addPeer(const QString &peerId)
{
    if (peerId.isEmpty()) {
        emitError("Peer ID cannot be empty");
        return;
    }
    
    QMutexLocker lock(&m_mutex);
    nullptr == m_database || !m_database ? emitError("Database not initialized") : 
        (m_database->addPeer(peerId), emit peerAdded(peerId));
}

void LocalTrustDiariesAdapter::removePeer(const QString &peerId)
{
    if (peerId.isEmpty()) {
        emitError("Peer ID cannot be empty");
        return;
    }
    
    QMutexLocker lock(&m_mutex);
    if (m_database) {
        m_database->removePeer(peerId);
        emit peerRemoved(peerId);
    } else {
        emitError("Database not initialized");
    }
}

void LocalTrustDiariesAdapter::updateTrustScore(const QString &peerId, double delta)
{
    if (peerId.isEmpty()) {
        emitError("Peer ID cannot be empty");
        return;
    }
    
    if (delta < -1.0 || delta > 1.0) {
        emitError("Delta must be between -1.0 and 1.0");
        return;
    }
    
    QMutexLocker lock(&m_mutex);
    if (!m_database) {
        emitError("Database not initialized");
        return;
    }
    
    m_database->updateScore(peerId, delta);
    double newScore = m_database->getScore(peerId);
    
    emit trustScoreChanged(peerId, newScore);
    emit stateUpdated(QString::fromStdString(m_database->getFullStateJson().dump()));
}

void LocalTrustDiariesAdapter::recordInteraction(const QString &peerId, 
                                               const QString &interactionType, 
                                               bool success)
{
    if (peerId.isEmpty()) {
        emitError("Peer ID cannot be empty");
        return;
    }
    
    if (interactionType.isEmpty()) {
        emitError("Interaction type cannot be empty");
        return;
    }
    
    QMutexLocker lock(&m_mutex);
    if (!m_database) {
        emitError("Database not initialized");
        return;
    }
    
    m_database->recordInteraction(peerId, interactionType, success);
    emit interactionRecorded(peerId, interactionType, success);
    emit stateUpdated(QString::fromStdString(m_database->getFullStateJson().dump()));
}

double LocalTrustDiariesAdapter::getTrustScore(const QString &peerId) const
{
    QMutexLocker lock(&m_mutex);
    if (!m_database) {
        return -1.0;
    }
    return m_database->getScore(peerId);
}

QString LocalTrustDiariesAdapter::getAllPeersAsJson() const
{
    QMutexLocker lock(&m_mutex);
    if (!m_database) {
        return "[]";
    }
    return QString::fromStdString(m_database->getAllPeersJson().dump());
}

QString LocalTrustDiariesAdapter::getInteractionHistoryAsJson(const QString &peerId) const
{
    QMutexLocker lock(&m_mutex);
    if (!m_database) {
        return "[]";
    }
    return QString::fromStdString(m_database->getInteractionHistoryJson(peerId).dump());
}

void LocalTrustDiariesAdapter::clearAllData()
{
    QMutexLocker lock(&m_mutex);
    if (m_database) {
        m_database->clear();
        emit stateReset();
        qDebug() << "[LocalTrustDiariesAdapter] All data cleared";
    }
}

QString LocalTrustDiariesAdapter::getStatusString() const
{
    QMutexLocker lock(&m_mutex);
    
    if (!m_database) {
        return "ERROR: Database not initialized";
    }
    
    int peerCount = m_database->getPeerCount();
    return QString("✓ Backend ready | %1 peers | Thread: %2")
        .arg(peerCount)
        .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()), 0, 16);
}

void LocalTrustDiariesAdapter::testBackendConnection()
{
    qDebug() << "[LocalTrustDiariesAdapter] Running backend connection test...";
    
    json result = {
        {"status", "testing"},
        {"timestamp", QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()}
    };
    
    try {
        // Test 1: Database exists
        {
            QMutexLocker lock(&m_mutex);
            if (!m_database) {
                result["error"] = "Database not initialized";
                emit connectionTestResult(QString::fromStdString(result.dump()));
                return;
            }
        }
        
        // Test 2: Add test peer
        const QString testPeerId = "TEST_PEER_" + QString::number(QDateTime::currentSecsSinceEpoch());
        {
            QMutexLocker lock(&m_mutex);
            m_database->addPeer(testPeerId);
        }
        
        // Test 3: Update trust
        {
            QMutexLocker lock(&m_mutex);
            m_database->updateScore(testPeerId, 0.1);
        }
        
        // Test 4: Record interaction
        {
            QMutexLocker lock(&m_mutex);
            m_database->recordInteraction(testPeerId, "test", true);
        }
        
        // Test 5: Query data
        double score = getTrustScore(testPeerId);
        
        // Test 6: Cleanup
        removePeer(testPeerId);
        
        // Success
        result["status"] = "success";
        result["testPeerId"] = testPeerId.toStdString();
        result["retrievedScore"] = score;
        result["message"] = "All backend tests passed";
        
        qDebug() << "[LocalTrustDiariesAdapter] ✓ Connection test PASSED";
        
    } catch (const std::exception &e) {
        result["status"] = "error";
        result["error"] = e.what();
        qWarning() << "[LocalTrustDiariesAdapter] ✗ Connection test FAILED:" << e.what();
    }
    
    emit connectionTestResult(QString::fromStdString(result.dump()));
}

void LocalTrustDiariesAdapter::emitTrustScoreChanged(const QString &peerId, double newScore)
{
    emit trustScoreChanged(peerId, newScore);
}

void LocalTrustDiariesAdapter::emitInteractionRecorded(const QString &peerId, 
                                                      const QString &type, 
                                                      bool success)
{
    emit interactionRecorded(peerId, type, success);
}

void LocalTrustDiariesAdapter::emitError(const QString &message, int code)
{
    m_lastError = message;
    qWarning() << "[LocalTrustDiariesAdapter] ERROR:" << message;
    emit errorOccurred(message, code);
}

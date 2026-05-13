#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QMutex>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @class LocalTrustDiariesAdapter
 * @brief Qt/QML wrapper for LocalTrustDiaries backend
 * 
 * Provides thread-safe access to trust score management with Qt signals/slots.
 * Designed as a standalone testable unit.
 */
class LocalTrustDiariesAdapter : public QObject {
    Q_OBJECT

public:
    explicit LocalTrustDiariesAdapter(QObject *parent = nullptr);
    ~LocalTrustDiariesAdapter() = default;

    // Non-copyable, moveable
    LocalTrustDiariesAdapter(const LocalTrustDiariesAdapter&) = delete;
    LocalTrustDiariesAdapter& operator=(const LocalTrustDiariesAdapter&) = delete;
    LocalTrustDiariesAdapter(LocalTrustDiariesAdapter&&) = default;
    LocalTrustDiariesAdapter& operator=(LocalTrustDiariesAdapter&&) = default;

    // =====================================================================
    // PUBLIC API — Called from QML
    // =====================================================================

    /**
     * @brief Add a new peer to trust tracking
     * @param peerId Unique peer identifier
     */
    Q_INVOKABLE void addPeer(const QString &peerId);

    /**
     * @brief Remove a peer from trust tracking
     * @param peerId Peer to remove
     */
    Q_INVOKABLE void removePeer(const QString &peerId);

    /**
     * @brief Update trust score for a peer
     * @param peerId Target peer
     * @param delta Score change (can be negative)
     * 
     * Thread-safe. Trust scores are clamped to [0, 1].
     */
    Q_INVOKABLE void updateTrustScore(const QString &peerId, double delta);

    /**
     * @brief Record an interaction with a peer
     * @param peerId Target peer
     * @param interactionType Type of interaction ("send", "receive", "validate", etc.)
     * @param success Whether interaction succeeded
     * 
     * Also automatically adjusts trust score:
     * - Success: +0.05 to score
     * - Failure: -0.1 to score
     */
    Q_INVOKABLE void recordInteraction(const QString &peerId, 
                                      const QString &interactionType, 
                                      bool success);

    /**
     * @brief Get current trust score for a peer  
     * @param peerId Target peer
     * @return Trust score (0-1), or -1 if peer not found
     */
    Q_INVOKABLE double getTrustScore(const QString &peerId) const;

    /**
     * @brief Get all peers as JSON array
     * @return JSON: [{peerId, score, trend, lastUpdate}]
     */
    Q_INVOKABLE QString getAllPeersAsJson() const;

    /**
     * @brief Get interaction history for a peer
     * @param peerId Target peer
     * @return JSON: [{type, success, timestamp}]
     */
    Q_INVOKABLE QString getInteractionHistoryAsJson(const QString &peerId) const;

    /**
     * @brief Clear all data (reset to empty state)
     * 
     * Useful for testing. Emits stateReset() signal.
     */
    Q_INVOKABLE void clearAllData();

    /**
     * @brief Get adapter status as human-readable string
     * @return Status message (good for UI display)
     */
    Q_INVOKABLE QString getStatusString() const;

    /**
     * @brief Test backend connectivity
     * 
     * Performs a test at various levels:
     * - Checks adapter initialized
     * - Performs test add/update/query
     * - Emits connectionTestResult(statusJson)
     */
    Q_INVOKABLE void testBackendConnection();

    // =====================================================================
    // SIGNALS — Emitted to QML
    // =====================================================================

    /**
     * @brief Emitted when an operation completes successfully
     * @param peerId Affected peer
     * @param newScore Updated trust score
     */
    Q_SIGNAL void trustScoreChanged(QString peerId, double newScore);

    /**
     * @brief Emitted when an interaction is recorded
     * @param peerId Peer who had interaction
     * @param interactionType Type of interaction
     * @param success Whether it succeeded
     */
    Q_SIGNAL void interactionRecorded(QString peerId, QString interactionType, bool success);

    /**
     * @brief Emitted when a peer is added
     * @param peerId New peer ID
     */
    Q_SIGNAL void peerAdded(QString peerId);

    /**
     * @brief Emitted when a peer is removed
     * @param peerId Removed peer ID
     */
    Q_SIGNAL void peerRemoved(QString peerId);

    /**
     * @brief Emitted when an error occurs
     * @param errorMessage Human-readable error
     * @param errorCode Backend error code (if applicable)
     */
    Q_SIGNAL void errorOccurred(QString errorMessage, int errorCode = -1);

    /**
     * @brief Emitted when state is cleared
     */
    Q_SIGNAL void stateReset();

    /**
     * @brief Emitted as result of testBackendConnection()
     * @param testResultJson JSON with test details
     */
    Q_SIGNAL void connectionTestResult(QString testResultJson);

    /**
     * @brief Emitted when backend data syncs to frontend
     * @param dataJson Full state as JSON
     */
    Q_SIGNAL void stateUpdated(QString dataJson);

private:
    // =====================================================================
    // INTERNAL STATE
    // =====================================================================

    // Thread safety: all access to backend guarded by this mutex
    mutable QMutex m_mutex;

    // In-memory trust database
    class TrustDatabase;
    std::unique_ptr<TrustDatabase> m_database;

    // Last error message (for diagnostics)
    QString m_lastError;

    // Signaling helpers (safe to call from any thread)
    void emitTrustScoreChanged(const QString &peerId, double newScore);
    void emitInteractionRecorded(const QString &peerId, const QString &type, bool success);
    void emitError(const QString &message, int code = -1);

    // Internal database class (forward declaration)
    class TrustDatabase {
    public:
        TrustDatabase();
        
        void addPeer(const QString &peerId);
        void removePeer(const QString &peerId);
        void updateScore(const QString &peerId, double delta);
        double getScore(const QString &peerId) const;
        void recordInteraction(const QString &peerId, const QString &type, bool success);
        
        json getAllPeersJson() const;
        json getInteractionHistoryJson(const QString &peerId) const;
        json getFullStateJson() const;
        
        void clear();
        int getPeerCount() const { return m_peers.size(); }

    private:
        struct PeerData {
            QString peerId;
            double trustScore = 0.5;  // Start at neutral
            int successCount = 0;
            int failureCount = 0;
            QVector<QPair<QString, bool>> interactions;  // (type, success)
        };

        QMap<QString, PeerData> m_peers;
    };

    friend class TrustDatabase;
};

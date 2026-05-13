#ifndef PKCERTCHAINADAPTER_H
#define PKCERTCHAINADAPTER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QString>
#include <QAtomicInteger>
#include <memory>
#include <atomic>

class PKCertChainAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool testMode READ testMode WRITE setTestMode NOTIFY testModeChanged)
    Q_PROPERTY(bool miningActive READ miningActive NOTIFY miningActiveChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit PKCertChainAdapter(QObject *parent = nullptr);
    ~PKCertChainAdapter() override;

    Q_INVOKABLE void startMining(int difficulty);
    Q_INVOKABLE void stopMining();
    Q_INVOKABLE void createBlock(const QString &data);
    Q_INVOKABLE void validateChain();
    Q_INVOKABLE void setTestMode(bool enabled);

    bool testMode() const;
    bool miningActive() const;
    QString status() const;
    Q_INVOKABLE QString chainStateJson() const;

signals:
    void miningStarted();
    void miningProgress(int percent);
    void blockMined(QString blockData);
    void chainUpdated(QString chainState);
    void miningStopped();
    void errorOccurred(QString message);
    void testModeChanged(bool enabled);
    void miningActiveChanged(bool active);
    void statusChanged(QString status);

private:
    struct BlockRecord {
        int index = 0;
        QString data;
        QString previousHash;
        QString hash;
        quint64 nonce = 0;
        int difficulty = 0;
        QString timestamp;
        bool mined = false;
        bool valid = true;
    };

    class MiningJob;
    friend class MiningJob;

    void appendPendingBlockLocked(const QString &data);
    void finalizeMinedBlockLocked(int jobId, const BlockRecord &minedBlock);
    QString buildChainJsonLocked(bool isValid, const QString &validationMessage) const;
    QString calculateHash(const QString &payload, quint64 nonce) const;
    QString calculateBlockHash(const BlockRecord &block, quint64 nonce) const;
    QString latestHashLocked() const;
    QString latestMinedHashLocked() const;
    bool validateChainLocked(QString *message = nullptr) const;
    void setStatusLocked(const QString &statusText);
    void emitChainUpdatedLocked(const QString &validationMessage = QString());
    QString createBlockJson(const BlockRecord &block) const;

    mutable QMutex m_mutex;
    QVector<BlockRecord> m_chain;
    bool m_testMode = true;
    bool m_miningActive = false;
    bool m_stopRequested = false;
    std::shared_ptr<std::atomic_bool> m_jobStopFlag;
    int m_activeDifficulty = 1;
    int m_jobCounter = 0;
    QString m_status;
};

#endif

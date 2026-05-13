#include "backend/PKCertChainAdapter.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QThreadPool>
#include <QRunnable>

class PKCertChainAdapter::MiningJob final : public QRunnable
{
public:
    MiningJob(PKCertChainAdapter *adapter,
              int jobId,
              int difficulty,
              BlockRecord block,
              bool testMode,
              std::shared_ptr<std::atomic_bool> stopFlag)
        : m_adapter(adapter),
          m_jobId(jobId),
          m_difficulty(difficulty),
          m_block(std::move(block)),
          m_testMode(testMode),
          m_stopFlag(std::move(stopFlag))
    {
        setAutoDelete(true);
    }

    void run() override
    {
        if (!m_adapter) {
            return;
        }

        const int effectiveDifficulty = m_testMode ? qMax(1, qMin(m_difficulty, 2)) : qMax(1, m_difficulty);
        const QString targetPrefix(QString(effectiveDifficulty, QChar('0')));
        const quint64 safetyLimit = m_testMode ? 25000 : 300000;
        quint64 nonce = 0;
        QString hash;
        bool mined = false;

        while (nonce < safetyLimit) {
            if (m_stopFlag && m_stopFlag->load()) {
                QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter]() {
                    QMutexLocker lock(&adapter->m_mutex);
                    adapter->m_miningActive = false;
                    adapter->m_stopRequested = false;
                    adapter->m_jobStopFlag.reset();
                    adapter->setStatusLocked(QStringLiteral("Mining stopped"));
                    emit adapter->miningStopped();
                    emit adapter->miningActiveChanged(false);
                    adapter->emitChainUpdatedLocked(QStringLiteral("Mining cancelled"));
                }, Qt::QueuedConnection);
                return;
            }

            hash = m_adapter->calculateBlockHash(m_block, nonce);

            if (m_testMode) {
                if ((nonce % 2000) == 0) {
                    const int percent = static_cast<int>((nonce * 100) / safetyLimit);
                    QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter, percent]() {
                        emit adapter->miningProgress(qMin(99, percent));
                    }, Qt::QueuedConnection);
                }

                if (nonce >= 1000) {
                    mined = true;
                    break;
                }
            } else {
                if (hash.startsWith(targetPrefix)) {
                    mined = true;
                    break;
                }

                if ((nonce % 5000) == 0) {
                    const int percent = static_cast<int>((nonce * 100) / safetyLimit);
                    QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter, percent]() {
                        emit adapter->miningProgress(qMin(99, percent));
                    }, Qt::QueuedConnection);
                }
            }

            ++nonce;
        }

        if (!mined) {
            if (m_stopFlag && m_stopFlag->load()) {
                QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter]() {
                    QMutexLocker lock(&adapter->m_mutex);
                    adapter->m_miningActive = false;
                    adapter->m_stopRequested = false;
                    adapter->m_jobStopFlag.reset();
                    adapter->setStatusLocked(QStringLiteral("Mining stopped"));
                    emit adapter->miningStopped();
                    emit adapter->miningActiveChanged(false);
                    adapter->emitChainUpdatedLocked(QStringLiteral("Mining cancelled"));
                }, Qt::QueuedConnection);
                return;
            }

            nonce = safetyLimit;
            hash = QStringLiteral("SIM-%1")
                       .arg(QString::fromLatin1(QCryptographicHash::hash((m_block.data + m_block.previousHash).toUtf8(), QCryptographicHash::Sha256).toHex().left(48)));
            mined = true;
            QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter]() {
                emit adapter->errorOccurred(QStringLiteral("Real mining exceeded safety limit, using simulation fallback"));
            }, Qt::QueuedConnection);
        }

        m_block.nonce = nonce;
        m_block.hash = hash;
        m_block.mined = mined;
        m_block.difficulty = m_difficulty;
        m_block.valid = true;
        m_block.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

        QMetaObject::invokeMethod(m_adapter, [adapter = m_adapter, jobId = m_jobId, minedBlock = m_block]() {
            QMutexLocker lock(&adapter->m_mutex);
            if (adapter->m_jobCounter != jobId) {
                return;
            }

            adapter->finalizeMinedBlockLocked(jobId, minedBlock);
            adapter->m_miningActive = false;
            adapter->m_stopRequested = false;
            adapter->m_jobStopFlag.reset();
            adapter->setStatusLocked(QStringLiteral("Mining completed"));
            emit adapter->miningProgress(100);
            emit adapter->miningStopped();
            emit adapter->miningActiveChanged(false);
            emit adapter->blockMined(adapter->createBlockJson(minedBlock));
            adapter->emitChainUpdatedLocked();
        }, Qt::QueuedConnection);
    }

private:
    PKCertChainAdapter *m_adapter = nullptr;
    int m_jobId = 0;
    int m_difficulty = 1;
    BlockRecord m_block;
    bool m_testMode = true;
    std::shared_ptr<std::atomic_bool> m_stopFlag;
};

PKCertChainAdapter::PKCertChainAdapter(QObject *parent)
    : QObject(parent)
{
    QMutexLocker lock(&m_mutex);
    setStatusLocked(QStringLiteral("Ready"));
    emit chainUpdated(buildChainJsonLocked(true, QStringLiteral("Chain initialized")));
}

PKCertChainAdapter::~PKCertChainAdapter()
{
    stopMining();
    QThreadPool::globalInstance()->waitForDone();
}

void PKCertChainAdapter::startMining(int difficulty)
{
    BlockRecord target;
    std::shared_ptr<std::atomic_bool> stopFlag;
    int jobId = 0;

    {
        QMutexLocker lock(&m_mutex);
        if (m_miningActive) {
            emit errorOccurred(QStringLiteral("Mining is already running"));
            return;
        }

        if (m_chain.isEmpty() || m_chain.last().mined) {
            appendPendingBlockLocked(QStringLiteral("Auto-mined block %1").arg(m_chain.size()));
        }

        m_miningActive = true;
        m_stopRequested = false;
        m_activeDifficulty = qMax(1, difficulty);
        ++m_jobCounter;
        jobId = m_jobCounter;

        target = m_chain.last();
        target.difficulty = m_activeDifficulty;
        target.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        m_chain.last().difficulty = m_activeDifficulty;
        m_chain.last().timestamp = target.timestamp;

        m_jobStopFlag = std::make_shared<std::atomic_bool>(false);
        stopFlag = m_jobStopFlag;
        setStatusLocked(QStringLiteral("Mining block #%1").arg(target.index));
    }

    QThreadPool::globalInstance()->start(new MiningJob(this, jobId, m_activeDifficulty, target, m_testMode, stopFlag));

    emit miningActiveChanged(true);
    emit miningStarted();
    emit chainUpdated(chainStateJson());
}

void PKCertChainAdapter::stopMining()
{
    QMutexLocker lock(&m_mutex);
    if (!m_miningActive) {
        return;
    }

    m_stopRequested = true;
    if (m_jobStopFlag) {
        m_jobStopFlag->store(true);
    }
    setStatusLocked(QStringLiteral("Stopping mining..."));
}

void PKCertChainAdapter::createBlock(const QString &data)
{
    QMutexLocker lock(&m_mutex);
    appendPendingBlockLocked(data.trimmed().isEmpty() ? QStringLiteral("Empty block payload") : data.trimmed());
    setStatusLocked(QStringLiteral("Block created and pending mining"));
    emitChainUpdatedLocked();
}

void PKCertChainAdapter::validateChain()
{
    QMutexLocker lock(&m_mutex);
    QString message;
    const bool valid = validateChainLocked(&message);
    setStatusLocked(valid ? QStringLiteral("Chain valid") : QStringLiteral("Chain invalid: %1").arg(message));
    emitChainUpdatedLocked(message);
}

void PKCertChainAdapter::setTestMode(bool enabled)
{
    QMutexLocker lock(&m_mutex);
    if (m_testMode == enabled) {
        return;
    }

    m_testMode = enabled;
    setStatusLocked(enabled ? QStringLiteral("Test mode active") : QStringLiteral("Real mining mode active"));
    emit testModeChanged(enabled);
    emitChainUpdatedLocked();
}

bool PKCertChainAdapter::testMode() const
{
    QMutexLocker lock(&m_mutex);
    return m_testMode;
}

bool PKCertChainAdapter::miningActive() const
{
    QMutexLocker lock(&m_mutex);
    return m_miningActive;
}

QString PKCertChainAdapter::status() const
{
    QMutexLocker lock(&m_mutex);
    return m_status;
}

QString PKCertChainAdapter::chainStateJson() const
{
    QMutexLocker lock(&m_mutex);
    return buildChainJsonLocked(validateChainLocked(), QString());
}

void PKCertChainAdapter::appendPendingBlockLocked(const QString &data)
{
    BlockRecord block;
    block.index = m_chain.size();
    block.data = data;
    block.previousHash = latestMinedHashLocked();
    block.hash = QStringLiteral("PENDING");
    block.nonce = 0;
    block.difficulty = m_activeDifficulty;
    block.timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    block.mined = false;
    block.valid = false;
    m_chain.append(block);
}

void PKCertChainAdapter::finalizeMinedBlockLocked(int jobId, const BlockRecord &minedBlock)
{
    Q_UNUSED(jobId)

    for (int i = m_chain.size() - 1; i >= 0; --i) {
        if (!m_chain[i].mined || m_chain[i].hash == QStringLiteral("PENDING")) {
            m_chain[i] = minedBlock;
            return;
        }
    }

    m_chain.append(minedBlock);
}

QString PKCertChainAdapter::buildChainJsonLocked(bool isValid, const QString &validationMessage) const
{
    QJsonObject root;
    root[QStringLiteral("status")] = m_status;
    root[QStringLiteral("testMode")] = m_testMode;
    root[QStringLiteral("miningActive")] = m_miningActive;
    root[QStringLiteral("activeDifficulty")] = m_activeDifficulty;
    root[QStringLiteral("isValid")] = isValid;
    root[QStringLiteral("validationMessage")] = validationMessage;

    QJsonArray blocks;
    for (const auto &block : m_chain) {
        QJsonObject entry;
        entry[QStringLiteral("index")] = block.index;
        entry[QStringLiteral("data")] = block.data;
        entry[QStringLiteral("previousHash")] = block.previousHash;
        entry[QStringLiteral("hash")] = block.hash;
        entry[QStringLiteral("nonce")] = static_cast<qint64>(block.nonce);
        entry[QStringLiteral("difficulty")] = block.difficulty;
        entry[QStringLiteral("timestamp")] = block.timestamp;
        entry[QStringLiteral("mined")] = block.mined;
        entry[QStringLiteral("valid")] = block.valid;
        blocks.append(entry);
    }

    root[QStringLiteral("blocks")] = blocks;
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QString PKCertChainAdapter::calculateHash(const QString &payload, quint64 nonce) const
{
    const QByteArray digest = QCryptographicHash::hash((payload + QString::number(nonce)).toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.toHex());
}

QString PKCertChainAdapter::calculateBlockHash(const BlockRecord &block, quint64 nonce) const
{
    const QString payload = QStringLiteral("%1|%2|%3|%4|%5")
                                .arg(block.index)
                                .arg(block.previousHash)
                                .arg(block.data)
                                .arg(block.timestamp)
                                .arg(m_testMode ? QStringLiteral("TEST") : QStringLiteral("REAL"));
    return calculateHash(payload, nonce);
}

QString PKCertChainAdapter::latestHashLocked() const
{
    return m_chain.isEmpty() ? QStringLiteral("0000000000000000") : m_chain.last().hash;
}

QString PKCertChainAdapter::latestMinedHashLocked() const
{
    for (int i = m_chain.size() - 1; i >= 0; --i) {
        if (m_chain[i].mined && m_chain[i].hash != QStringLiteral("PENDING")) {
            return m_chain[i].hash;
        }
    }
    return QStringLiteral("0000000000000000");
}

bool PKCertChainAdapter::validateChainLocked(QString *message) const
{
    if (m_chain.isEmpty()) {
        if (message) {
            *message = QStringLiteral("Chain is empty");
        }
        return true;
    }

    for (int i = 0; i < m_chain.size(); ++i) {
        const auto &block = m_chain[i];
        const QString expectedPrev = i == 0 ? QStringLiteral("0000000000000000") : m_chain[i - 1].hash;

        if (block.previousHash != expectedPrev) {
            if (message) {
                *message = QStringLiteral("Invalid previous hash at block %1").arg(block.index);
            }
            return false;
        }

        if (!block.mined || block.hash == QStringLiteral("PENDING")) {
            if (message) {
                *message = QStringLiteral("Pending block at index %1").arg(block.index);
            }
            return false;
        }

        if (!m_testMode) {
            const QString hash = calculateBlockHash(block, block.nonce);
            const QString targetPrefix(block.difficulty, QChar('0'));
            if (!hash.startsWith(targetPrefix)) {
                if (message) {
                    *message = QStringLiteral("Difficulty target not met at block %1").arg(block.index);
                }
                return false;
            }
        }
    }

    if (message) {
        *message = QStringLiteral("Chain validated successfully");
    }

    return true;
}

void PKCertChainAdapter::setStatusLocked(const QString &statusText)
{
    if (m_status != statusText) {
        m_status = statusText;
        emit statusChanged(m_status);
    }
}

void PKCertChainAdapter::emitChainUpdatedLocked(const QString &validationMessage)
{
    const bool valid = validateChainLocked();
    emit chainUpdated(buildChainJsonLocked(valid, validationMessage));
}

QString PKCertChainAdapter::createBlockJson(const BlockRecord &block) const
{
    QJsonObject entry;
    entry[QStringLiteral("index")] = block.index;
    entry[QStringLiteral("data")] = block.data;
    entry[QStringLiteral("previousHash")] = block.previousHash;
    entry[QStringLiteral("hash")] = block.hash;
    entry[QStringLiteral("nonce")] = static_cast<qint64>(block.nonce);
    entry[QStringLiteral("difficulty")] = block.difficulty;
    entry[QStringLiteral("timestamp")] = block.timestamp;
    entry[QStringLiteral("mined")] = block.mined;
    entry[QStringLiteral("valid")] = block.valid;
    return QString::fromUtf8(QJsonDocument(entry).toJson(QJsonDocument::Compact));
}

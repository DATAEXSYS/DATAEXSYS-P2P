#ifndef SIMULATORENGINE_H
#define SIMULATORENGINE_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>
#include <QDateTime>

class SimulatorEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool attackMode READ attackMode WRITE setAttackMode NOTIFY attackModeChanged)
    Q_PROPERTY(bool trustEnabled READ trustEnabled WRITE setTrustEnabled NOTIFY togglesChanged)
    Q_PROPERTY(bool certEnabled READ certEnabled WRITE setCertEnabled NOTIFY togglesChanged)
    Q_PROPERTY(bool wormholeEnabled READ wormholeEnabled WRITE setWormholeEnabled NOTIFY togglesChanged)
    Q_PROPERTY(bool dsrEnabled READ dsrEnabled WRITE setDsrEnabled NOTIFY togglesChanged)
    Q_PROPERTY(bool rollingEnabled READ rollingEnabled WRITE setRollingEnabled NOTIFY togglesChanged)

public:
    explicit SimulatorEngine(QObject *parent = nullptr);

    bool attackMode() const { return m_attackMode; }
    void setAttackMode(bool v) { if(m_attackMode != v) { m_attackMode = v; emit attackModeChanged(); } }

    bool trustEnabled() const { return m_trustEnabled; }
    void setTrustEnabled(bool v) { if(m_trustEnabled != v) { m_trustEnabled = v; emit togglesChanged(); } }

    bool certEnabled() const { return m_certEnabled; }
    void setCertEnabled(bool v) { if(m_certEnabled != v) { m_certEnabled = v; emit togglesChanged(); } }

    bool wormholeEnabled() const { return m_wormholeEnabled; }
    void setWormholeEnabled(bool v) { if(m_wormholeEnabled != v) { m_wormholeEnabled = v; emit togglesChanged(); } }

    bool dsrEnabled() const { return m_dsrEnabled; }
    void setDsrEnabled(bool v) { if(m_dsrEnabled != v) { m_dsrEnabled = v; emit togglesChanged(); } }

    bool rollingEnabled() const { return m_rollingEnabled; }
    void setRollingEnabled(bool v) { if(m_rollingEnabled != v) { m_rollingEnabled = v; emit togglesChanged(); } }

    Q_INVOKABLE QVariantList getTrustScores();
    Q_INVOKABLE QVariantList getCertChain();
    Q_INVOKABLE QVariantList getRouteLatencies();
    Q_INVOKABLE void sendMessage(const QString &from, const QString &to, const QString &msg);
    Q_INVOKABLE void simulatePow();

signals:
    void attackModeChanged();
    void togglesChanged();
    void messageSent(QVariantMap msgData);
    void trustUpdated();
    void blockMined(QVariantMap block);
    void anomalyDetected(QString reason);

private slots:
    void onSimulationTick();

private:
    bool m_attackMode = false;
    bool m_trustEnabled = false;
    bool m_certEnabled = false;
    bool m_wormholeEnabled = false;
    bool m_dsrEnabled = false;
    bool m_rollingEnabled = false;

    QTimer *m_tickTimer;
    
    struct TrustNode {
        double alpha = 1.0;
        double beta = 1.0;
    };
    QMap<QString, TrustNode> m_trustDb;

    struct Block {
        QString hash;
        QString timestamp;
    };
    QList<Block> m_blockchain;
};

#endif

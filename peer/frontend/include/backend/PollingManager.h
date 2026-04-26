#ifndef POLLINGMANAGER_H
#define POLLINGMANAGER_H

#include <QObject>
#include <QTimer>

class PollingManager : public QObject {
    Q_OBJECT

public:
    explicit PollingManager(QObject *parent = nullptr);

    void start();
    void stop();

    void setBaseInterval(int msec) { m_baseInterval = msec; }

public slots:
    void onActivityDetected();
    void onNoActivityDetected();

signals:
    void tick();

private slots:
    void handleTimeout();

private:
    QTimer *m_timer;
    int m_baseInterval = 5000;
    int m_activeInterval = 2000;
    int m_currentInterval;
    int m_ticksWithoutChange = 0;
};

#endif

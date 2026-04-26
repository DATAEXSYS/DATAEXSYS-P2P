#include "backend/PollingManager.h"
#include <QDebug>

PollingManager::PollingManager(QObject *parent)
    : QObject(parent), m_currentInterval(m_baseInterval) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PollingManager::handleTimeout);
}

void PollingManager::start() {
    m_timer->start(m_currentInterval);
}

void PollingManager::stop() {
    m_timer->stop();
}

void PollingManager::onActivityDetected() {
    m_ticksWithoutChange = 0;
    if (m_currentInterval != m_activeInterval) {
        m_currentInterval = m_activeInterval;
        m_timer->setInterval(m_currentInterval);
        qDebug() << "[Polling] Speeding up to" << m_currentInterval << "ms";
    }
}

void PollingManager::onNoActivityDetected() {
    m_ticksWithoutChange++;
    if (m_ticksWithoutChange > 5 && m_currentInterval != m_baseInterval) {
        m_currentInterval = m_baseInterval;
        m_timer->setInterval(m_currentInterval);
        qDebug() << "[Polling] Slowing down to" << m_currentInterval << "ms";
    }
}

void PollingManager::handleTimeout() {
    emit tick();
}

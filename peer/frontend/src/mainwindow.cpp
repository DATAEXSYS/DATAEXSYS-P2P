#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ================= UI =================
    panel = new NetworkPanel(this);
    setCentralWidget(panel);

    // ================= SERVICE =================
    service = new NetworkService(this);

    // ================= CONNECT UI → SERVICE =================
    connect(panel->refreshButton(), &QPushButton::clicked,
            service, &NetworkService::loadNetworks);

    // ================= SERVICE → UI =================
    connect(service, &NetworkService::networksUpdated,
            panel, &NetworkPanel::setNetworks);

    connect(service, &NetworkService::error,
            this, [=](QString err) {
        qDebug() << "Service error:" << err;
    });

    // optional: auto-load on start
    service->loadNetworks();
}
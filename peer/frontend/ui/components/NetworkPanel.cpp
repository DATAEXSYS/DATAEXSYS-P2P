#include "ui/components/NetworkPanel.h"
#include <QVBoxLayout>
#include <QHeaderView>

NetworkPanel::NetworkPanel(QWidget *parent)
    : QWidget(parent)
{
    loadBtn = new QPushButton("Load Networks");
    table = new QTableWidget();

    setupTable();

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(loadBtn);
    layout->addWidget(table);

    setLayout(layout);
}

// ================= TABLE SETUP =================
void NetworkPanel::setupTable() {
    table->setColumnCount(5);

    QStringList headers;
    headers << "Network ID"
            << "Name"
            << "Type"
            << "Peers"
            << "Status";

    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// ================= SET DATA =================
void NetworkPanel::setNetworks(const QVector<Network> &networks) {
    table->setRowCount(networks.size());

    for (int i = 0; i < networks.size(); i++) {
        const Network &n = networks[i];

        table->setItem(i, 0, new QTableWidgetItem(n.networkId));
        table->setItem(i, 1, new QTableWidgetItem(n.name));
        table->setItem(i, 2, new QTableWidgetItem(n.type));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(n.activePeers)));

        table->setItem(i, 4, new QTableWidgetItem(
            n.active ? "ACTIVE" : "OFFLINE"
        ));
    }
}
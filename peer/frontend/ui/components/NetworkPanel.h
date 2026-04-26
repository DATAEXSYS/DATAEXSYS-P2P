#ifndef NETWORKPANEL_H
#define NETWORKPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QVector>

#include "model/Network.h"

class NetworkPanel : public QWidget {
    Q_OBJECT

public:
    explicit NetworkPanel(QWidget *parent = nullptr);

    QPushButton* refreshButton() { return loadBtn; }

    void setNetworks(const QVector<Network> &networks);

private:
    QPushButton *loadBtn;
    QTableWidget *table;

    void setupTable();
};

#endif
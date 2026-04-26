#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    manager = new QNetworkAccessManager(this);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *btn = new QPushButton("Load Networks");
    auto *output = new QTextEdit();

    layout->addWidget(btn);
    layout->addWidget(output);

    setCentralWidget(central);

    connect(btn, &QPushButton::clicked, this, [=]() {
        QUrl url("http://localhost:8080/api/networks");
        QNetworkRequest request(url);

        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [=]() {
            output->setText(reply->readAll());
            reply->deleteLater();
        });
    });
}
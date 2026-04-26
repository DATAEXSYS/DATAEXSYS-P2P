#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui/components/NetworkPanel.h"
#include "service/NetworkService.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    NetworkPanel *panel;
    NetworkService *service;
};

#endif
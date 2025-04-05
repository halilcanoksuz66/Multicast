#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // MulticastManager'ı başlat
    multicastManager = new MulticastManager(this);
    connect(multicastManager, &MulticastManager::messageReceived, this, &MainWindow::onMessageReceived);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete multicastManager;
}

void MainWindow::onMessageReceived(const QString& message) {
    ui->messageDisplay->setText(message);
}

void MainWindow::onSendButtonClicked() {
    QString message = ui->messageInput->text();
    multicastManager->sendMessage(message);
}


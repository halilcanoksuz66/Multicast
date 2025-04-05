#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multicast_receiver.h"
#include "multicast_sender.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Receiver başlat
    receiverThread = new QThread(this);
    multicastReceiver = new MulticastReceiver();
    multicastReceiver->moveToThread(receiverThread);

    connect(receiverThread, &QThread::started, multicastReceiver, &MulticastReceiver::start);
    connect(multicastReceiver, &MulticastReceiver::messageReceived, this, &MainWindow::onMessageReceived);

    receiverThread->start();


    // Senderi başlat
    multicastSender = new MulticastSender();
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

}

MainWindow::~MainWindow()
{
    receiverThread->quit();
    receiverThread->wait();

    delete multicastReceiver;
    delete multicastSender;
    delete ui;
}

void MainWindow::onMessageReceived(const QString& message) {
    ui->messageDisplay->setText(message);
}

void MainWindow::onSendButtonClicked() {
    QString message = ui->messageInput->text();

    multicastSender->sendMessage(message);
}


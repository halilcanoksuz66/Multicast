#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audiocapture.h"
#include <QDateTime>
#include <QDir>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioCapture(new AudioCapture())
{
    ui->setupUi(this);

    // MulticastManager'ı başlat
    multicastManager = new MulticastManager(this);
    connect(multicastManager, &MulticastManager::messageReceived, this, &MainWindow::onMessageReceived);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    // Start ve Stop butonlarına bağlantı ekle
    connect(ui->startButton, &QPushButton::clicked, audioCapture, &AudioCapture::startCapture);
    connect(ui->stopButton, &QPushButton::clicked, audioCapture, &AudioCapture::stopCapture);
    
    // Save butonuna bağlantı ekle
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    
    // Connect the saveRequested signal from AudioCapture
    connect(audioCapture, &AudioCapture::saveRequested, this, &MainWindow::onSaveButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete multicastManager;
    delete audioCapture;
}

void MainWindow::onMessageReceived(const QString& message) {
    ui->messageDisplay->setText(message);
}

void MainWindow::onSendButtonClicked() {
    QString message = ui->messageInput->text();
    multicastManager->sendMessage(message);
}

void MainWindow::onSaveButtonClicked() {
    // Proje klasöründe recordings alt klasörü oluştur
    QDir dir;
    if (!dir.exists("recordings")) {
        dir.mkdir("recordings");
    }

    // Dosya adını tarih ve saat ile oluştur
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    QString filename = QString("recordings/recording_%1.wav").arg(timestamp);

    // Ses verisini kaydet
    if (audioCapture->saveToWav(filename)) {
        ui->messageDisplay->setText("Ses kaydedildi: " + filename);
    } else {
        ui->messageDisplay->setText("Ses kaydedilemedi!");
    }
}

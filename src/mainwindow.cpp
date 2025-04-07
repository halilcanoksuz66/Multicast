#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioCapture(new AudioCapture(this))
    , multicastManager(new MulticastManager(this))
    , audioCodec(new AudioCodec(this))
{
    ui->setupUi(this);

    // UI bağlantıları
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);

    // AudioCapture bağlantıları
    connect(audioCapture, &AudioCapture::audioCaptured, multicastManager, &MulticastManager::onAudioCaptured);
    connect(audioCapture, &AudioCapture::saveRequested, this, &MainWindow::onSaveRequested);

    // MulticastManager bağlantıları
    connect(multicastManager, &MulticastManager::audioReceived, this, &MainWindow::onAudioReceived);
    connect(multicastManager, &MulticastManager::messageReceived, this, &MainWindow::onMessageReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartButtonClicked()
{
    audioCapture->startCapture();
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void MainWindow::onStopButtonClicked()
{
    audioCapture->stopCapture();
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void MainWindow::onSaveRequested()
{
    QString filename = QFileDialog::getSaveFileName(this,
        "Ses Dosyasını Kaydet", "",
        "WAV Dosyaları (*.wav);;Tüm Dosyalar (*)");
    
    if (!filename.isEmpty()) {
        audioCapture->saveToWav(filename);
    }
}

void MainWindow::onAudioReceived(std::vector<char> data)
{
    // Alınan ses verisini işle
    qDebug() << "Ses verisi alındı, boyut:" << data.size() << "byte";
    
    // Sıkıştırılmış veriyi çöz
    std::vector<char> decompressedData = audioCodec->decompress(data);
    
    // Çözülmüş veriyi çal
    if (audioCapture) {
       // audioCapture->playAudio(decompressedData);
    }
}

void MainWindow::onMessageReceived(const QString &message)
{
    ui->statusBar->showMessage(message);
}

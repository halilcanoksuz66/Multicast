#include "multicast_manager.h"
#include <QDebug>

MulticastManager::MulticastManager(QObject *parent)
    : QObject(parent)
{
    // Receiver başlat
    receiverThread = new QThread(this);
    multicastReceiver = new MulticastReceiver();
    multicastReceiver->moveToThread(receiverThread);

    connect(receiverThread, &QThread::started, multicastReceiver, &MulticastReceiver::start);
    
    // Alınan ses verilerini işlemek için bağlantı kur
    connect(multicastReceiver, &MulticastReceiver::audioReceived, this, &MulticastManager::audioReceived);

    receiverThread->start();
    emit messageReceived("Multicast alıcı başlatıldı");

    // Senderi başlat
    multicastSender = new MulticastSender();
    emit messageReceived("Multicast gönderici başlatıldı");

    // Audio codec'i başlat
    audioCodec = new AudioCodec(this);
    emit messageReceived("Ses codec'i başlatıldı");
}

MulticastManager::~MulticastManager()
{
    receiverThread->quit();
    receiverThread->wait();

    delete multicastReceiver;
    delete multicastSender;
    delete audioCodec;
    emit messageReceived("Multicast bağlantıları kapatıldı");
}

void MulticastManager::sendAudio(std::vector<char> capturedData) {
    // Ses verisini sıkıştır
    std::vector<char> compressedData = audioCodec->compress(capturedData);
    
    // Sıkıştırılmış veriyi gönder
    multicastSender->sendAudio(compressedData);
    emit messageReceived(QString("Sıkıştırılmış ses verisi gönderildi (%1 byte)").arg(compressedData.size()));
}

void MulticastManager::onAudioCaptured(std::vector<char> data) {
    // Ses verisini multicast ile gönder
    sendAudio(data);
    
    // Loglama
    qDebug() << "Ses verisi gönderildi, boyut:" << data.size() << "byte";
}


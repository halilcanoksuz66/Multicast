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
}

MulticastManager::~MulticastManager()
{
    receiverThread->quit();
    receiverThread->wait();

    delete multicastReceiver;
    delete multicastSender;
    emit messageReceived("Multicast bağlantıları kapatıldı");
}

void MulticastManager::sendAudio(std::vector<char> capturedData) {
    multicastSender->sendAudio(capturedData);
    emit messageReceived(QString("Ses verisi gönderildi (%1 byte)").arg(capturedData.size()));
}

void MulticastManager::onAudioCaptured(std::vector<char> data) {
    // Ses verisini multicast ile gönder
    sendAudio(data);
}


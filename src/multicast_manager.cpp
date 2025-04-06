#include "multicast_manager.h"

MulticastManager::MulticastManager(QObject *parent)
    : QObject(parent)
{
    // Receiver başlat
    receiverThread = new QThread(this);
    multicastReceiver = new MulticastReceiver();
    multicastReceiver->moveToThread(receiverThread);

    connect(receiverThread, &QThread::started, multicastReceiver, &MulticastReceiver::start);
    connect(multicastReceiver, &MulticastReceiver::messageReceived, this, &MulticastManager::messageReceived);

    receiverThread->start();

    // Senderi başlat
    multicastSender = new MulticastSender();
}

MulticastManager::~MulticastManager()
{
    receiverThread->quit();
    receiverThread->wait();

    delete multicastReceiver;
    delete multicastSender;
}

void MulticastManager::sendMessage(const QString& message)
{
    multicastSender->sendMessage(message);
} 
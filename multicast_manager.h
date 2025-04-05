#ifndef MULTICAST_MANAGER_H
#define MULTICAST_MANAGER_H

#include "multicast_receiver.h"
#include "multicast_sender.h"
#include <QObject>
#include <QThread>

class MulticastManager : public QObject
{
    Q_OBJECT

public:
    explicit MulticastManager(QObject *parent = nullptr);
    ~MulticastManager();

    void sendMessage(const QString& message);

signals:
    void messageReceived(const QString& message);

private:
    MulticastReceiver *multicastReceiver;
    MulticastSender* multicastSender;
    QThread *receiverThread;
};

#endif // MULTICAST_MANAGER_H 
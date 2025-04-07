#ifndef MULTICAST_MANAGER_H
#define MULTICAST_MANAGER_H

#include "multicast_receiver.h"
#include "multicast_sender.h"
#include "audio_codec.h"
#include <QObject>
#include <QThread>

class MulticastManager : public QObject
{
    Q_OBJECT

public:
    explicit MulticastManager(QObject *parent = nullptr);
    ~MulticastManager();
    void sendAudio(std::vector<char> capturedData);

signals:
    void messageReceived(const QString &message);
    void audioReceived(std::vector<char> data);

public slots:
    void onAudioCaptured(std::vector<char> data);

private:
    MulticastReceiver *multicastReceiver;
    MulticastSender* multicastSender;
    AudioCodec* audioCodec;
    QThread *receiverThread;
};

#endif // MULTICAST_MANAGER_H

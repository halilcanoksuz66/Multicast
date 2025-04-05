#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H

#include <QObject>
#include <asio.hpp>
#include <QString>

using asio::ip::udp;

class MulticastSender : public QObject {
    Q_OBJECT

public:
    MulticastSender();
    void sendMessage(const QString &message);
    void start();

private:
    asio::io_context io_context;
    udp::socket socket;
};

#endif // MULTICASTSENDER_H

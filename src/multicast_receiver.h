#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H

#include <QObject>
#include <asio.hpp>
#include <QString>

using asio::ip::udp;

class MulticastReceiver : public QObject {
    Q_OBJECT

public:
    explicit MulticastReceiver(QObject *parent = nullptr);
    void start();

signals:
    void messageReceived(const QString &message);

private:
    void receive();
    void handleReceive(const asio::error_code &error, std::size_t bytes_received);

    asio::io_context io_context;
    udp::socket socket;
    udp::endpoint sender_endpoint;
    enum { max_length = 1024 };
    char data[max_length];
};

#endif // MULTICASTRECEIVER_H

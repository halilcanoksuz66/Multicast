#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H

#include <QObject>
#include <asio.hpp>
#include <QString>
#include <QDebug>

using asio::ip::udp;

class MulticastReceiver : public QObject {
    Q_OBJECT

public:
    explicit MulticastReceiver(QObject *parent = nullptr);
    void start();

signals:
    void audioReceived(std::vector<char> data);

private:
    void receive();
    void handleReceive(const asio::error_code &error, std::size_t bytes_received);

    asio::io_context io_context;
    udp::socket socket;
    udp::endpoint sender_endpoint;
    std::vector<char> receive_buffer{std::vector<char>(1024)}; // 1024 byte'lık buffer

};

#endif // MULTICASTRECEIVER_H

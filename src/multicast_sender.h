#ifndef MULTICAST_SENDER_H
#define MULTICAST_SENDER_H

#include <QObject>
#include <asio.hpp>
#include <vector>
#include <string>

using asio::ip::udp;

class MulticastSender : public QObject
{
    Q_OBJECT
public:
    explicit MulticastSender(QObject *parent = nullptr);
    ~MulticastSender();

    void sendAudio(std::vector<char> audioData);

private:
    asio::io_context io_context;
    udp::socket socket;
    udp::endpoint endpoint;
    std::string multicast_address;
    unsigned short multicast_port;
};

#endif // MULTICAST_SENDER_H

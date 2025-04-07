#include "multicast_sender.h"
#include <QDebug>

MulticastSender::MulticastSender(QObject *parent)
    : QObject(parent)
    , socket(io_context)
    , multicast_address("239.255.0.1")
    , multicast_port(12345)
{
    try {
        socket.open(udp::v4());
        endpoint = udp::endpoint(
            asio::ip::address::from_string(multicast_address),
            multicast_port
        );
    } catch (const std::exception& e) {
        qDebug() << "MulticastSender başlatma hatası:" << e.what();
    }
}

MulticastSender::~MulticastSender()
{
    if (socket.is_open()) {
        socket.close();
    }
}

void MulticastSender::sendAudio(std::vector<char> audioData)
{
    try {
        if (!socket.is_open()) {
            qDebug() << "Socket kapalı, yeniden açılıyor...";
            socket.open(udp::v4());
        }

        qDebug() << "Gönderilen ses verisi boyutu:" << audioData.size() << "byte";

        socket.send_to(asio::buffer(audioData), endpoint);
    } catch (const std::exception& e) {
        qDebug() << "Ses verisi gönderme hatası:" << e.what();
    }
}

#include "multicast_receiver.h"

const std::string MULTICAST_ADDRESS = "239.255.0.1";
const short PORT = 12345;

MulticastReceiver::MulticastReceiver(QObject *parent)
    : QObject(parent), socket(io_context)
{
    udp::endpoint listen_endpoint(udp::v4(), PORT);
    socket.open(listen_endpoint.protocol());
    socket.set_option(udp::socket::reuse_address(true));
    socket.bind(listen_endpoint);

    socket.set_option(
        asio::ip::multicast::join_group(
            asio::ip::address::from_string(MULTICAST_ADDRESS)));
}

void MulticastReceiver::start() {
    receive();
    io_context.run();
}

void MulticastReceiver::receive() {
    socket.async_receive_from(
        asio::buffer(receive_buffer),
        sender_endpoint,
        [this](const asio::error_code &error, std::size_t bytes_received) {
            this->handleReceive(error, bytes_received);
        });
}

void MulticastReceiver::handleReceive(const asio::error_code &error, std::size_t bytes_received) {
    if (!error) {
        // Alınan ses verisini işle
        std::vector<char> receivedData(receive_buffer.begin(), receive_buffer.begin() + bytes_received);
        
        // Debug mesajı
        qDebug() << "Alınan ses verisi boyutu:" << receivedData.size() << "byte";
        
        // Ses verisini işlemek için bir sinyal yayınla
        emit audioReceived(receivedData);
        
        // Bir sonraki veriyi almak için receive metodunu tekrar çağır
        receive();
    } else {
        // Hata durumunda loglama yap
        qDebug() << "Multicast alım hatası:" << QString::fromStdString(error.message());
        
        // Hata durumunda da tekrar dinlemeye devam et
        receive();
    }
}

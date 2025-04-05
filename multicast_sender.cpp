#include "multicast_sender.h"
#include <asio/ip/multicast.hpp>

const std::string MULTICAST_ADDRESS = "239.255.0.1";
const short PORT = 12345;


MulticastSender::MulticastSender() : socket(io_context) {
    socket.open(udp::v4());
    socket.set_option(udp::socket::reuse_address(true));
}

void MulticastSender::sendMessage(const QString &message) {
    udp::endpoint endpoint(asio::ip::address::from_string(MULTICAST_ADDRESS), PORT);
    socket.send_to(asio::buffer(message.toStdString()), endpoint);
}

void MulticastSender::start() {
    io_context.run();
}

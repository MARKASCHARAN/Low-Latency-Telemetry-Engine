#include "llte/network/udp_receiver.hpp"

#include <arpa/inet.h>
#include <unistd.h>

UdpReceiver::UdpReceiver(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(sockfd, (sockaddr*)&addr, sizeof(addr));
}

UdpReceiver::~UdpReceiver() {
    close(sockfd);
}

bool UdpReceiver::receive(TelemetryPacket& packet) {
    auto bytes = recvfrom(
        sockfd,
        &packet,
        sizeof(packet),
        0,
        nullptr,
        nullptr
    );

    return bytes == sizeof(packet);
    
}
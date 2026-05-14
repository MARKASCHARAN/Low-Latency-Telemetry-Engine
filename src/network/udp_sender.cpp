#include "llte/network/udp_sender.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

UdpSender::UdpSender(const std::string& ip, int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr = new sockaddr_in{};
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);

    inet_pton(AF_INET, ip.c_str(), &server_addr->sin_addr);
}

UdpSender::~UdpSender() {
    close(sockfd);
    delete server_addr;
}

bool UdpSender::send(const TelemetryPacket& packet) {
    auto bytes = sendto(
        sockfd,
        &packet,
        sizeof(packet),
        0,
        (sockaddr*)server_addr,
        sizeof(*server_addr)
    );

    return bytes == sizeof(packet);
}
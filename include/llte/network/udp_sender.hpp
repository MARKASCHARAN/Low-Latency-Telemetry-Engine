#pragma once

#include <string>
#include "llte/telemetry/packet.hpp"

class UdpSender {
public:
    UdpSender(const std::string& ip, int port);
    ~UdpSender();

    bool send(const TelemetryPacket& packet);

private:
    int sockfd;
    struct sockaddr_in* server_addr;
};
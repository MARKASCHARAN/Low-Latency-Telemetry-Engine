#pragma once

#include "llte/telemetry/packet.hpp"

class UdpReceiver {
public:
    explicit UdpReceiver(int port);
    ~UdpReceiver();

    bool receive(TelemetryPacket& packet);

private:
    int sockfd;
};
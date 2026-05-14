#include <iostream>

#include "llte/network/udp_receiver.hpp"

int main() {
    UdpReceiver receiver(8080);

    TelemetryPacket packet;

    while (true) {
        if (receiver.receive(packet)) {
            std::cout
                << "Received | Speed: " << packet.speed
                << " | RPM: " << packet.rpm
                << " | Gear: " << packet.gear
                << " | Fuel: " << packet.fuel
                << "\n";
        }
    }
}
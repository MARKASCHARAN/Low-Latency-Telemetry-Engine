#include <chrono>
#include <iostream>
#include <random>
#include <thread>

// Required headers
#include "llte/network/udp_sender.hpp"
#include "llte/telemetry/packet.hpp"

int main() {
    UdpSender sender("127.0.0.1", 8080);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> speed_dist(100.0f, 340.0f);
    std::uniform_real_distribution<float> temp_dist(80.0f, 120.0f);

    while (true) {
        TelemetryPacket packet{};

        packet.timestamp =
            std::chrono::duration_cast<
                std::chrono::milliseconds>(
                std::chrono::system_clock::now()
                    .time_since_epoch()
            ).count();

        packet.speed = speed_dist(gen);
        packet.rpm = 12000 + rand() % 3000;
        packet.gear = 1 + rand() % 8;

        packet.throttle = 60 + rand() % 40;
        packet.brake = rand() % 100;

        packet.tire_temp_fl = temp_dist(gen);
        packet.tire_temp_fr = temp_dist(gen);
        packet.tire_temp_rl = temp_dist(gen);
        packet.tire_temp_rr = temp_dist(gen);

        packet.fuel = 50.0f;

        sender.send(packet);

        std::cout
            << "Sent | Speed: "
            << packet.speed
            << " km/h\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(10)
        );
    }

    return 0;
}
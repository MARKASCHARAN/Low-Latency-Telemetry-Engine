#include "llte/core/engine.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "llte/network/udp_receiver.hpp"
#include "llte/queue/spsc_queue.hpp"

constexpr size_t QUEUE_SIZE = 1024;

SPSCQueue<TelemetryPacket, QUEUE_SIZE> queue;

std::atomic<bool> running = true;

std::atomic<uint64_t> dropped_packets = 0;

void network_thread() {

    UdpReceiver receiver(8080);

    while (running) {

        TelemetryPacket packet{};

        if (receiver.receive(packet)) {

            if (!queue.push(packet)) {

                dropped_packets++;
            }
        }
    }
}

void processing_thread() {

    uint64_t processed = 0;

    auto start =
        std::chrono::steady_clock::now();

    while (running) {

        TelemetryPacket packet{};

        if (queue.pop(packet)) {

            processed++;

            if (processed % 1000 == 0) {

                auto elapsed =
                    std::chrono::duration_cast<
                        std::chrono::milliseconds>(
                        std::chrono::steady_clock::now()
                        - start
                    ).count();

                std::cout
                    << "\n=== LLTE Metrics ===\n"
                    << "Processed: "
                    << processed
                    << "\nQueue Size: "
                    << queue.size()
                    << "\nDropped: "
                    << dropped_packets.load()
                    << "\nElapsed: "
                    << elapsed
                    << " ms\n\n";
            }
        }
    }
}

void Engine::start() {

    std::thread net(network_thread);

    std::thread proc(processing_thread);

    net.join();

    proc.join();
}
#include "llte/core/engine.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

#include "llte/network/udp_receiver.hpp"
#include "llte/queue/spsc_queue.hpp"

constexpr size_t QUEUE_SIZE = 1024;
constexpr size_t LATENCY_WINDOW = 10000;

SPSCQueue<TelemetryPacket, QUEUE_SIZE> queue;

alignas(64) std::atomic<bool> running = true;
alignas(64) std::atomic<uint64_t> dropped_packets = 0;
alignas(64) std::atomic<size_t> max_queue_depth = 0;

void network_thread()
{
    UdpReceiver receiver(8080);

    while (running)
    {
        TelemetryPacket packet{};

        if (receiver.receive(packet))
        {
            if (!queue.push(packet))
            {
                dropped_packets++;
            }

            size_t current_size = queue.size();
            size_t current_max = max_queue_depth.load();

            if (current_size > current_max)
            {
                max_queue_depth.store(current_size);
            }
        }
    }
}

void processing_thread()
{
    uint64_t processed = 0;

    uint64_t total_latency_us = 0;
    uint64_t min_latency_us = std::numeric_limits<uint64_t>::max();
    uint64_t max_latency_us = 0;

    std::vector<uint64_t> latencies;
    latencies.reserve(LATENCY_WINDOW);

    auto start = std::chrono::steady_clock::now();

    while (running)
    {
        TelemetryPacket packet{};

        if (queue.pop(packet))
        {
            processed++;

            uint64_t now_us =
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                    std::chrono::steady_clock::now()
                        .time_since_epoch())
                    .count();

            uint64_t latency_us =
                now_us - packet.timestamp;

            // bounded rolling buffer
            if (latencies.size() >= LATENCY_WINDOW)
            {
                latencies.clear();   // O(1), keeps reserved capacity
            }

            latencies.push_back(latency_us);

            total_latency_us += latency_us;

            if (latency_us < min_latency_us)
            {
                min_latency_us = latency_us;
            }

            if (latency_us > max_latency_us)
            {
                max_latency_us = latency_us;
            }

            if (processed % 1000 == 0)
            {
                auto elapsed =
                    std::chrono::duration_cast<
                        std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();

                uint64_t avg_latency =
                    total_latency_us / processed;

                double packets_per_sec =
                    (processed * 1000.0) / elapsed;

                auto sorted = latencies;
                std::sort(
                    sorted.begin(),
                    sorted.end()
                );

                size_t p50_idx =
                    sorted.size() * 50 / 100;

                size_t p95_idx =
                    sorted.size() * 95 / 100;

                size_t p99_idx =
                    sorted.size() * 99 / 100;

                uint64_t p50 =
                    sorted[p50_idx];

                uint64_t p95 =
                    sorted[p95_idx];

                uint64_t p99 =
                    sorted[p99_idx];

                std::cout
                    << "\n=== LLTE Metrics ===\n"
                    << "Processed: "
                    << processed
                    << "\nPackets/sec: "
                    << packets_per_sec
                    << "\nQueue Size: "
                    << queue.size()
                    << "\nMax Queue Depth: "
                    << max_queue_depth.load()
                    << "/"
                    << QUEUE_SIZE
                    << "\nDropped: "
                    << dropped_packets.load()
                    << "\nAvg Latency: "
                    << avg_latency
                    << " us"
                    << "\nMin Latency: "
                    << min_latency_us
                    << " us"
                    << "\nP50 Latency: "
                    << p50
                    << " us"
                    << "\nP95 Latency: "
                    << p95
                    << " us"
                    << "\nP99 Latency: "
                    << p99
                    << " us"
                    << "\nMax Latency: "
                    << max_latency_us
                    << " us"
                    << "\nElapsed: "
                    << elapsed
                    << " ms\n\n";
            }
        }
    }
}

void Engine::start()
{
    std::cout << "LLTE Engine starting...\n";

    std::thread net(network_thread);
    std::thread proc(processing_thread);

    net.join();
    proc.join();
}
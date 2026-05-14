#pragma once

#include <array>
#include <atomic>
#include <cstddef>

template<typename T, size_t Capacity>
class SPSCQueue {
public:
    SPSCQueue() : head_(0), tail_(0) {}

    bool push(const T& item) {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t next = (head + 1) % Capacity;

        if (next == tail_.load(std::memory_order_acquire)) {
            return false; // queue full
        }

        buffer_[head] = item;

        head_.store(next, std::memory_order_release);

        return true;
    }

    bool pop(T& item) {
        const size_t tail = tail_.load(std::memory_order_relaxed);

        if (tail == head_.load(std::memory_order_acquire)) {
            return false; // queue empty
        }

        item = buffer_[tail];

        tail_.store(
            (tail + 1) % Capacity,
            std::memory_order_release
        );

        return true;
    }

    size_t size() const {
        const size_t head = head_.load();
        const size_t tail = tail_.load();

        if (head >= tail)
            return head - tail;

        return Capacity - tail + head;
    }

private:
    std::array<T, Capacity> buffer_;

    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
};
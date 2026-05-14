#include <cassert>

#include "llte/queue/spsc_queue.hpp"

int main() {

    SPSCQueue<int, 8> queue;

    assert(queue.push(42));

    int value;

    assert(queue.pop(value));

    assert(value == 42);

    return 0;
}
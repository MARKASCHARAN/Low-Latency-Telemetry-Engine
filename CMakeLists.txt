cmake_minimum_required(VERSION 3.20)

project(llte)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# =========================
# Core library
# =========================
add_library(llte_core
    src/network/udp_sender.cpp
    src/network/udp_receiver.cpp
    src/core/engine.cpp
)

# =========================
# Simulator
# =========================
add_executable(llte_simulator
    apps/simulator/main.cpp
)

target_link_libraries(
    llte_simulator
    llte_core
)

# =========================
# Receiver
# =========================
add_executable(llte_receiver
    apps/receiver/main.cpp
)

target_link_libraries(
    llte_receiver
    llte_core
)

# =========================
# Tests
# =========================
add_executable(queue_test
    tests/queue_test.cpp
)

target_link_libraries(
    queue_test
    llte_core
)
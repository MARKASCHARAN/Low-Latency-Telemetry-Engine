cmake_minimum_required(VERSION 3.20)
project(llte)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(llte_network
    src/network/udp_sender.cpp
    src/network/udp_receiver.cpp
)

add_executable(llte_simulator
    apps/simulator/main.cpp
)

target_link_libraries(llte_simulator llte_network)

add_executable(llte_receiver
    apps/receiver/main.cpp
)

target_link_libraries(llte_receiver llte_network)
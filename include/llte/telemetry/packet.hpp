#pragma once

#include <cstdint>

struct TelemetryPacket {
    uint64_t timestamp;

    float speed;
    int rpm;
    int gear;

    float throttle;
    float brake;

    float tire_temp_fl;
    float tire_temp_fr;
    float tire_temp_rl;
    float tire_temp_rr;

    float fuel;
};
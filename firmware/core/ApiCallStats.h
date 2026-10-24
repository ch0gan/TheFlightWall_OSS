#pragma once

#include <Arduino.h>

namespace ApiCallStats
{
    enum class Provider : uint8_t
    {
        FlightRadar24,
        Adsbdb,
        Hexdb,
        AeroApi
    };

    struct Snapshot
    {
        uint32_t fr24 = 0;
        uint32_t adsbdb = 0;
        uint32_t hexdb = 0;
        uint32_t aeroApi = 0;
        uint32_t windowSeconds = 0;
        bool fullDay = false;
    };

    void begin();
    void record(Provider provider);
    Snapshot snapshot();
}

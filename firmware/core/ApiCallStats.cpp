#include "core/ApiCallStats.h"

#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

namespace
{
    constexpr uint16_t kWindowMinutes = 24 * 60;
    constexpr uint32_t kWindowSeconds = 24 * 60 * 60;
    constexpr size_t kProviderCount = 4;

    struct MinuteBucket
    {
        uint16_t calls[kProviderCount] = {};
    };

    MinuteBucket g_buckets[kWindowMinutes] = {};
    portMUX_TYPE g_mux = portMUX_INITIALIZER_UNLOCKED;
    uint32_t g_lastMillis = 0;
    uint64_t g_uptimeMs = 0;
    uint64_t g_advancedMinute = 0;
    uint16_t g_currentSlot = 0;
    bool g_initialized = false;

    void advanceLocked(uint32_t nowMs)
    {
        if (!g_initialized)
        {
            g_lastMillis = nowMs;
            g_initialized = true;
            return;
        }

        g_uptimeMs += static_cast<uint32_t>(nowMs - g_lastMillis);
        g_lastMillis = nowMs;
        const uint64_t targetMinute = g_uptimeMs / 60000ULL;
        const uint64_t elapsedMinutes = targetMinute - g_advancedMinute;
        if (elapsedMinutes >= kWindowMinutes)
        {
            memset(g_buckets, 0, sizeof(g_buckets));
            g_currentSlot = static_cast<uint16_t>(targetMinute % kWindowMinutes);
        }
        else
        {
            for (uint64_t i = 0; i < elapsedMinutes; ++i)
            {
                g_currentSlot = static_cast<uint16_t>((g_currentSlot + 1) % kWindowMinutes);
                memset(&g_buckets[g_currentSlot], 0, sizeof(g_buckets[g_currentSlot]));
            }
        }
        g_advancedMinute = targetMinute;
    }

    size_t providerIndex(ApiCallStats::Provider provider)
    {
        return static_cast<size_t>(provider);
    }
}

namespace ApiCallStats
{
    void begin()
    {
        portENTER_CRITICAL(&g_mux);
        memset(g_buckets, 0, sizeof(g_buckets));
        g_lastMillis = millis();
        g_uptimeMs = 0;
        g_advancedMinute = 0;
        g_currentSlot = 0;
        g_initialized = true;
        portEXIT_CRITICAL(&g_mux);
    }

    void record(Provider provider)
    {
        portENTER_CRITICAL(&g_mux);
        advanceLocked(millis());
        const size_t index = providerIndex(provider);
        if (index < kProviderCount && g_buckets[g_currentSlot].calls[index] < UINT16_MAX)
            ++g_buckets[g_currentSlot].calls[index];
        portEXIT_CRITICAL(&g_mux);
    }

    Snapshot snapshot()
    {
        Snapshot result;
        portENTER_CRITICAL(&g_mux);
        advanceLocked(millis());
        for (const MinuteBucket &bucket : g_buckets)
        {
            result.fr24 += bucket.calls[providerIndex(Provider::FlightRadar24)];
            result.adsbdb += bucket.calls[providerIndex(Provider::Adsbdb)];
            result.hexdb += bucket.calls[providerIndex(Provider::Hexdb)];
            result.aeroApi += bucket.calls[providerIndex(Provider::AeroApi)];
        }
        result.fullDay = g_uptimeMs >= static_cast<uint64_t>(kWindowSeconds) * 1000ULL;
        result.windowSeconds = result.fullDay ? kWindowSeconds : static_cast<uint32_t>(g_uptimeMs / 1000ULL);
        portEXIT_CRITICAL(&g_mux);
        return result;
    }
}

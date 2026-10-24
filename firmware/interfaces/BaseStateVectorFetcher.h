#pragma once

#include <Arduino.h>
#include <vector>
#include "models/StateVector.h"

struct WatchBandCounts
{
    uint32_t inner = 0;
    uint32_t middle = 0;
    uint32_t outer = 0;
};

class BaseStateVectorFetcher
{
public:
    virtual ~BaseStateVectorFetcher() = default;

    virtual bool fetchStateVectors(
        double centerLat,
        double centerLon,
        double radiusKm,
        std::vector<StateVector> &outStateVectors) = 0;

    virtual WatchBandCounts lastWatchBandCounts() const { return {}; }

    // Position of one named aircraft anywhere, not limited to a radius. Distance/bearing are
    // measured from the given point. Returns false if unsupported or the request failed.
    virtual bool fetchByIdent(const String &ident,
                              double fromLat,
                              double fromLon,
                              std::vector<StateVector> &outStateVectors)
    {
        (void)ident; (void)fromLat; (void)fromLon; (void)outStateVectors;
        return false;
    }
};

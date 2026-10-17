#pragma once
// Arduino-free pure helpers (host-testable): distance to go and route progress.
#include <cmath>
#include "GeoUtils.h"

// Routes shorter than this give a meaningless bar (airport coordinates are only ~km-accurate).
constexpr double kMinRouteKm = 20.0;

// Great-circle km from the aircraft to the destination; NAN if any input is missing.
inline double distanceToGoKm(double lat, double lon, double destLat, double destLon)
{
    if (!std::isfinite(lat) || !std::isfinite(lon) ||
        !std::isfinite(destLat) || !std::isfinite(destLon))
        return NAN;
    return haversineKm(lat, lon, destLat, destLon);
}

// Percent of the route flown, 0-100. flown/(flown+togo) rather than 1-togo/total,
// so a detour or holding pattern can never push it outside 0-100.
inline double routeProgressPct(double originLat, double originLon,
                               double destLat, double destLon,
                               double lat, double lon)
{
    if (!std::isfinite(originLat) || !std::isfinite(originLon))
        return NAN;
    const double togo = distanceToGoKm(lat, lon, destLat, destLon);
    if (!std::isfinite(togo) || !std::isfinite(lat) || !std::isfinite(lon))
        return NAN;
    const double flown = haversineKm(originLat, originLon, lat, lon);
    const double total = flown + togo;
    if (total < kMinRouteKm)
        return NAN;
    return 100.0 * flown / total;
}

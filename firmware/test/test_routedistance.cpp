// Host unit tests for RouteDistance.h -- compile with g++, no hardware.
//
// Guarded like the other loose host tests under test/; see platformio.ini.
#ifndef PIO_UNIT_TESTING
#include "../utils/RouteDistance.h"
#include <cstdio>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

int main() {
    // SFO -> SIN, aircraft at the start, near the end, and missing.
    const double oLat = 37.619, oLon = -122.375, dLat = 1.35019, dLon = 103.994;

    CHECK(std::fabs(routeProgressPct(oLat, oLon, dLat, dLon, oLat, oLon)) < 0.01);
    CHECK(std::fabs(routeProgressPct(oLat, oLon, dLat, dLon, dLat, dLon) - 100.0) < 0.01);

    // Halfway on the great circle (midpoint of the arc) reads ~50.
    const double mid = routeProgressPct(oLat, oLon, dLat, dLon, 52.0, -173.0);
    CHECK(mid > 0.0 && mid < 100.0);

    // Off-route detours stay inside 0-100.
    const double off = routeProgressPct(oLat, oLon, dLat, dLon, -40.0, 0.0);
    CHECK(off >= 0.0 && off <= 100.0);

    // Missing airport or aircraft position means no bar and no DOG.
    CHECK(std::isnan(routeProgressPct(NAN, NAN, dLat, dLon, 10.0, 10.0)));
    CHECK(std::isnan(routeProgressPct(oLat, oLon, NAN, NAN, 10.0, 10.0)));
    CHECK(std::isnan(routeProgressPct(oLat, oLon, dLat, dLon, NAN, NAN)));
    CHECK(std::isnan(distanceToGoKm(NAN, 0, 0, 0)));

    // A route shorter than kMinRouteKm is noise, not a bar.
    CHECK(std::isnan(routeProgressPct(40.0, -74.0, 40.01, -74.0, 40.0, -74.0)));

    if (failures == 0) printf("test_routedistance: all passed\n");
    return failures ? 1 : 0;
}
#endif

#pragma once
// Arduino-free (host-testable): approximate coordinates for common airports, keyed by IATA.
// Seeds the learned table in FlightDataFetcher so a bar can draw before adsbdb has named the airport.
#include <cstring>

struct SeedAirport
{
    const char *iata;
    double lat;
    double lon;
};

inline bool seedAirportCoord(const char *iata, double &lat, double &lon)
{
    static const SeedAirport kSeed[] = {
        {"EWR", 40.6925, -74.1687}, {"JFK", 40.6413, -73.7781}, {"LGA", 40.7769, -73.8740},
        {"PHL", 39.8744, -75.2424}, {"TTN", 40.2767, -74.8135}, {"ACY", 39.4576, -74.5772},
        {"ABE", 40.6521, -75.4408}, {"BOS", 42.3656, -71.0096}, {"BDL", 41.9389, -72.6832},
        {"ALB", 42.7483, -73.8017}, {"SYR", 43.1112, -76.1063}, {"ROC", 43.1189, -77.6724},
        {"BUF", 42.9405, -78.7322}, {"PIT", 40.4915, -80.2329}, {"DCA", 38.8512, -77.0402},
        {"IAD", 38.9531, -77.4565}, {"BWI", 39.1754, -76.6683}, {"SBY", 38.3405, -75.5103},
        {"RDU", 35.8776, -78.7875}, {"GSO", 36.0978, -79.9373}, {"AVL", 35.4362, -82.5418},
        {"CLT", 35.2140, -80.9431}, {"ATL", 33.6407, -84.4277}, {"JAX", 30.4941, -81.6879},
        {"MCO", 28.4312, -81.3081}, {"TPA", 27.9755, -82.5332}, {"MIA", 25.7959, -80.2870},
        {"FLL", 26.0726, -80.1527}, {"DTW", 42.2162, -83.3554}, {"CMH", 39.9980, -82.8919},
        {"IND", 39.7173, -86.2944}, {"SDF", 38.1744, -85.7360}, {"ORD", 41.9742, -87.9073},
        {"MDW", 41.7868, -87.7522}, {"MSP", 44.8848, -93.2223}, {"MCI", 39.2976, -94.7139},
        {"DFW", 32.8998, -97.0403}, {"IAH", 29.9902, -95.3368}, {"DEN", 39.8561, -104.6737},
        {"PHX", 33.4342, -112.0116}, {"LAS", 36.0840, -115.1537}, {"LAX", 33.9416, -118.4085},
        {"SFO", 37.6213, -122.3790}, {"SJC", 37.3626, -121.9291}, {"SMF", 38.6954, -121.5908},
        {"SEA", 47.4502, -122.3088},
    };
    if (!iata || !*iata)
        return false;
    for (const SeedAirport &a : kSeed)
    {
        if (strcmp(a.iata, iata) == 0)
        {
            lat = a.lat;
            lon = a.lon;
            return true;
        }
    }
    return false;
}

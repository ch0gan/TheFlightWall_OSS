// Host unit tests for FlightClassify.h — compile with g++, no hardware.
//
// Guarded so a `pio test` build doesn't collide with the other loose host
// tests under test/ -- see the test_filter comment in platformio.ini. This
// file is a standalone host test; it only runs via bare g++, which never
// defines PIO_UNIT_TESTING, so the guard is a no-op for that workflow and
// this file's behavior there is unchanged.
#ifndef PIO_UNIT_TESTING
#include "../utils/FlightClassify.h"
#include <cstdio>
#include <cstring>
#include <string>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

int main() {
    // isTailNumber() — positive registration signal
    CHECK(isTailNumber("N172SP"));   // US N-number
    CHECK(isTailNumber("N1"));       // minimal N-number (N + digit)
    CHECK(isTailNumber("N12345"));
    CHECK(isTailNumber("C-GABC"));   // hyphenated international registration

    // Airline-format callsigns must NEVER be classified as tail
    CHECK(!isTailNumber("DAL123"));
    CHECK(!isTailNumber("AAL2960"));
    CHECK(!isTailNumber("GTI4521"));  // Atlas: airline-format -> not a tail

    // Empty / null
    CHECK(!isTailNumber(""));
    CHECK(!isTailNumber(nullptr));

    // Whitespace tolerated on both ends
    CHECK(isTailNumber("  N99 "));

    // isCargoOperator() — curated freight-operator set
    CHECK(isCargoOperator("FDX"));
    CHECK(isCargoOperator("UPS"));
    CHECK(isCargoOperator("GTI"));
    CHECK(isCargoOperator("GEC"));
    CHECK(isCargoOperator("CLX"));
    CHECK(isCargoOperator("CKS"));
    CHECK(isCargoOperator("CAO"));
    CHECK(isCargoOperator("BOX"));
    CHECK(isCargoOperator("ABW"));
    CHECK(isCargoOperator("MPH"));
    CHECK(isCargoOperator("PAC"));
    CHECK(isCargoOperator("GSS"));
    CHECK(isCargoOperator("NCA"));
    CHECK(isCargoOperator("CKK"));
    CHECK(isCargoOperator("SQC"));
    CHECK(isCargoOperator("ABX"));
    CHECK(isCargoOperator("CJT"));
    CHECK(isCargoOperator("BCS"));
    CHECK(isCargoOperator("DHK"));

    // Non-cargo passenger operators
    CHECK(!isCargoOperator("DAL"));
    CHECK(!isCargoOperator("AAL"));
    CHECK(!isCargoOperator("UAL"));
    CHECK(!isCargoOperator("JBU"));
    CHECK(!isCargoOperator(""));
    CHECK(!isCargoOperator(nullptr));

    // Case-insensitive
    CHECK(isCargoOperator("fdx"));

    // isGeneralAviation() — the GA/private side of the airline split. This is
    // the predicate the "show general aviation" setting keys off on EVERY
    // source, so it has to agree with Area mode's two passes exactly: whatever
    // pass 1 (airline-format) refuses must land here, and nothing else.
    CHECK(isGeneralAviation("N172SP"));   // US N-number
    CHECK(isGeneralAviation("C-GABC"));   // hyphenated international registration
    CHECK(isGeneralAviation("N1"));

    // Broader than isTailNumber on purpose: a callsign that is neither
    // airline-format NOR registration-shaped is still not an airline flight,
    // and Area mode's pass 1 has always rejected it. Were this keyed on
    // isTailNumber instead, these would slip through a filter set to hide them.
    CHECK(isGeneralAviation("GOLF12"));
    CHECK(!isTailNumber("GOLF12"));       // ...and it is NOT a tail number
    CHECK(isGeneralAviation("LIFEGUARD"));
    CHECK(isGeneralAviation(""));
    CHECK(isGeneralAviation(nullptr));    // unknown is not an airline flight

    // Airline flights are never GA -- these must survive the filter.
    CHECK(!isGeneralAviation("DAL123"));
    CHECK(!isGeneralAviation("AAL2960"));
    CHECK(!isGeneralAviation("GTI4521"));  // cargo is an airline flight, not GA
    CHECK(!isGeneralAviation("QFA3"));     // shortest airline form: 3 letters + 1 digit
    CHECK(!isGeneralAviation("  BAW286 ")); // leading space tolerated, as in pass 1

    // ICAO aircraft type plus enrichment labels select a generic aircraft visual.
    CHECK(classifyAircraftIcon("C172") == AircraftIconClass::Cessna);
    CHECK(classifyAircraftIcon("GLF5") == AircraftIconClass::BusinessJet);
    CHECK(classifyAircraftIcon("R44") == AircraftIconClass::Helicopter);
    CHECK(classifyAircraftIcon("H25B") == AircraftIconClass::BusinessJet);
    CHECK(classifyAircraftIcon("ZZZZ") == AircraftIconClass::Unknown);
    CHECK(classifyAircraftIcon("ZZZZ", "Cessna", "172S Skyhawk") == AircraftIconClass::Cessna);
    CHECK(classifyAircraftIcon("", "Textron Aviation", "172S Skyhawk SP") == AircraftIconClass::Cessna);
    CHECK(classifyAircraftIcon("C208") == AircraftIconClass::Cessna);
    CHECK(classifyAircraftIcon("C310") == AircraftIconClass::Cessna);
    // Citations are Cessnas but stay business jets.
    CHECK(classifyAircraftIcon("C25A") == AircraftIconClass::BusinessJet);
    CHECK(classifyAircraftIcon("C56X", "Cessna", "Citation Excel") == AircraftIconClass::BusinessJet);
    CHECK(classifyAircraftIcon("ZZZZ", "Cessna", "Citation CJ3") == AircraftIconClass::BusinessJet);
    // "C + digits" is not enough: these are military transports.
    CHECK(classifyAircraftIcon("C130") == AircraftIconClass::Unknown);
    CHECK(classifyAircraftIcon("C17") == AircraftIconClass::Unknown);
    // Other light planes are Light, not Cessna.
    CHECK(classifyAircraftIcon("P28A") == AircraftIconClass::Light);
    CHECK(classifyAircraftIcon("SR22") == AircraftIconClass::Light);
    CHECK(classifyAircraftIcon("ZZZZ", "Piper", "PA-28-181 Archer") == AircraftIconClass::Light);
    CHECK(classifyAircraftIcon("ZZZZ", "Gulfstream Aerospace", "G550") == AircraftIconClass::BusinessJet);
    CHECK(classifyAircraftIcon("AA5") == AircraftIconClass::Light);
    CHECK(classifyAircraftIcon("ZZZZ", "Grumman American", "AA-5 Tiger") == AircraftIconClass::Light);

    // Logo choice, shared by the LED and the web preview.
    {
        const char *c[kMaxLogoCandidates];
        auto first = [&](bool heli, bool priv, const char *op, bool cargo, const char *cls) {
            return logoCandidates(heli, priv, op, cargo, cls, c) ? std::string(c[0]) : std::string();
        };
        CHECK(first(false, true, "", false, "cessna") == "_CESSNA");     // N172SP
        CHECK(first(false, true, "", false, "light") == "_LIGHT");       // a Piper by tail number
        CHECK(first(false, true, "", false, "business") == "_BIZJET");
        CHECK(first(false, true, "", false, "") == "_PRIVATE");          // type unknown
        CHECK(first(true, true, "", false, "cessna") == "_HELI");        // helicopter wins
        CHECK(first(false, false, "EJA", false, "business") == "EJA");   // operator tile first
        // ...and the class tile is the fallback when that operator has none.
        size_t n = logoCandidates(false, false, "EJA", false, "business", c);
        CHECK(n == 2 && std::string(c[1]) == "_BIZJET");
        n = logoCandidates(false, false, "FDX", true, "", c);
        CHECK(n == 2 && std::string(c[0]) == "FDX" && std::string(c[1]) == "_CARGO");
        // Private + known type: the unknown icon is still there as a fallback.
        n = logoCandidates(false, true, "", false, "cessna", c);
        CHECK(n == 2 && std::string(c[1]) == "_PRIVATE");
    }

    if (failures == 0) { printf("ALL PASS\n"); return 0; }
    printf("%d FAILURES\n", failures);
    return 1;
}
#endif // PIO_UNIT_TESTING

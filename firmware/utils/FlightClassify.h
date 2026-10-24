#pragma once
// Arduino-free pure helpers (host-testable). No String, no Arduino.h.
//
// Two POSITIVE classifiers used to identify private/GA and cargo flights.
//
// CRITICAL design rule: "private/tail" must be a POSITIVE signal — the callsign
// itself looks like an aircraft registration. It must NEVER mean "enrichment
// failed". Airline-format callsigns (3 letters + a digit, e.g. DAL123, GTI4521)
// must NEVER be classified as a tail number.
#include <cctype>
#include <cstddef>
#include <cstring>
#include "CallsignUtils.h"

enum class AircraftIconClass : unsigned char
{
    Unknown,
    Light,
    BusinessJet,
    Helicopter
};

inline bool aircraftTextContains(const char *text, const char *word)
{
    if (!text || !word)
        return false;
    const size_t wordLength = std::strlen(word);
    for (const char *start = text; *start; ++start)
    {
        size_t i = 0;
        while (i < wordLength && start[i] &&
               std::tolower((unsigned char)start[i]) == std::tolower((unsigned char)word[i]))
            ++i;
        if (i == wordLength)
            return true;
    }
    return false;
}

inline bool aircraftTypeStartsWith(const char *type, const char *prefix)
{
    if (!type || !prefix)
        return false;
    while (*type == ' ')
        ++type;
    for (size_t i = 0; prefix[i]; ++i)
        if (!type[i] || std::toupper((unsigned char)type[i]) != prefix[i])
            return false;
    return true;
}

inline AircraftIconClass classifyAircraftIcon(const char *typeCode,
                                               const char *manufacturer = nullptr,
                                               const char *model = nullptr)
{
    if (aircraftTextContains(manufacturer, "helicopter") ||
        aircraftTextContains(manufacturer, "robinson") ||
        aircraftTextContains(manufacturer, "sikorsky") ||
        aircraftTextContains(manufacturer, "leonardo") ||
        aircraftTextContains(manufacturer, "eurocopter") ||
        aircraftTextContains(manufacturer, "bell helicopter"))
        return AircraftIconClass::Helicopter;

    if (aircraftTypeStartsWith(typeCode, "R22") || aircraftTypeStartsWith(typeCode, "R44") ||
        aircraftTypeStartsWith(typeCode, "R66") || aircraftTypeStartsWith(typeCode, "B06") ||
        aircraftTypeStartsWith(typeCode, "B407") || aircraftTypeStartsWith(typeCode, "B429") ||
        aircraftTypeStartsWith(typeCode, "EC30") || aircraftTypeStartsWith(typeCode, "EC35") ||
        aircraftTypeStartsWith(typeCode, "EC45") || aircraftTypeStartsWith(typeCode, "EC55") ||
        aircraftTypeStartsWith(typeCode, "AS50") || aircraftTypeStartsWith(typeCode, "AS55") ||
        aircraftTypeStartsWith(typeCode, "AS65") || aircraftTypeStartsWith(typeCode, "S76") ||
        aircraftTypeStartsWith(typeCode, "S92") || aircraftTypeStartsWith(typeCode, "H125") ||
        aircraftTypeStartsWith(typeCode, "H130") || aircraftTypeStartsWith(typeCode, "H135") ||
        aircraftTypeStartsWith(typeCode, "H145") || aircraftTypeStartsWith(typeCode, "H160") ||
        aircraftTypeStartsWith(typeCode, "H175") || aircraftTypeStartsWith(typeCode, "H225"))
        return AircraftIconClass::Helicopter;

    if (aircraftTextContains(manufacturer, "gulfstream") ||
        aircraftTextContains(manufacturer, "bombardier") ||
        aircraftTextContains(manufacturer, "dassault aviation") ||
        aircraftTextContains(manufacturer, "learjet") ||
        aircraftTextContains(manufacturer, "hawker") ||
        aircraftTextContains(model, "citation") || aircraftTextContains(model, "gulfstream") ||
        aircraftTextContains(model, "learjet") || aircraftTextContains(model, "falcon") ||
        aircraftTextContains(model, "challenger") || aircraftTextContains(model, "global express"))
        return AircraftIconClass::BusinessJet;

    if (aircraftTypeStartsWith(typeCode, "GLF") || aircraftTypeStartsWith(typeCode, "G150") ||
        aircraftTypeStartsWith(typeCode, "G280") || aircraftTypeStartsWith(typeCode, "GLEX") ||
        aircraftTypeStartsWith(typeCode, "G650") || aircraftTypeStartsWith(typeCode, "G700") ||
        aircraftTypeStartsWith(typeCode, "C25") || aircraftTypeStartsWith(typeCode, "C56") ||
        aircraftTypeStartsWith(typeCode, "C68") || aircraftTypeStartsWith(typeCode, "C700") ||
        aircraftTypeStartsWith(typeCode, "CL30") || aircraftTypeStartsWith(typeCode, "CL35") ||
        aircraftTypeStartsWith(typeCode, "CL60") || aircraftTypeStartsWith(typeCode, "LJ") ||
        aircraftTypeStartsWith(typeCode, "FA50") || aircraftTypeStartsWith(typeCode, "FA7") ||
        aircraftTypeStartsWith(typeCode, "FA8") || aircraftTypeStartsWith(typeCode, "F2TH") ||
        aircraftTypeStartsWith(typeCode, "H25") || aircraftTypeStartsWith(typeCode, "PC24"))
        return AircraftIconClass::BusinessJet;

    if (aircraftTextContains(manufacturer, "cessna") || aircraftTextContains(manufacturer, "piper") ||
        aircraftTextContains(manufacturer, "cirrus") || aircraftTextContains(manufacturer, "diamond") ||
        aircraftTextContains(manufacturer, "mooney") || aircraftTextContains(manufacturer, "beechcraft") ||
        aircraftTextContains(manufacturer, "tecnam") || aircraftTextContains(manufacturer, "robin aircraft") ||
        aircraftTextContains(manufacturer, "grumman") || aircraftTextContains(manufacturer, "american general") ||
        aircraftTextContains(model, "skyhawk") || aircraftTextContains(model, "skylane") ||
        aircraftTextContains(model, "aa-5") || aircraftTextContains(model, "aa5") ||
        aircraftTextContains(model, "tiger"))
        return AircraftIconClass::Light;

    if (aircraftTypeStartsWith(typeCode, "AA5") || aircraftTypeStartsWith(typeCode, "C152") || aircraftTypeStartsWith(typeCode, "C162") ||
        aircraftTypeStartsWith(typeCode, "C172") || aircraftTypeStartsWith(typeCode, "C182") ||
        aircraftTypeStartsWith(typeCode, "C206") || aircraftTypeStartsWith(typeCode, "C208") ||
        aircraftTypeStartsWith(typeCode, "C210") || aircraftTypeStartsWith(typeCode, "PA2") ||
        aircraftTypeStartsWith(typeCode, "PA3") || aircraftTypeStartsWith(typeCode, "PA4") ||
        aircraftTypeStartsWith(typeCode, "PA46") || aircraftTypeStartsWith(typeCode, "P28") ||
        aircraftTypeStartsWith(typeCode, "SR20") || aircraftTypeStartsWith(typeCode, "SR22") ||
        aircraftTypeStartsWith(typeCode, "DA20") || aircraftTypeStartsWith(typeCode, "DA40") ||
        aircraftTypeStartsWith(typeCode, "DA42") || aircraftTypeStartsWith(typeCode, "BE33") ||
        aircraftTypeStartsWith(typeCode, "BE35") || aircraftTypeStartsWith(typeCode, "BE36") ||
        aircraftTypeStartsWith(typeCode, "BE55") || aircraftTypeStartsWith(typeCode, "BE58") ||
        aircraftTypeStartsWith(typeCode, "M20P"))
        return AircraftIconClass::Light;

    return AircraftIconClass::Unknown;
}

inline const char *aircraftIconClassName(AircraftIconClass iconClass)
{
    switch (iconClass)
    {
    case AircraftIconClass::Light:       return "light";
    case AircraftIconClass::BusinessJet: return "business";
    case AircraftIconClass::Helicopter:  return "heli";
    case AircraftIconClass::Unknown:     break;
    }
    return "";
}

// isTailNumber: positive "this is an aircraft registration, not an airline
// flight" test. Conservative — unknown/ambiguous input is NOT auto-private.
inline bool isTailNumber(const char *callsign)
{
    if (!callsign)
        return false;

    // Trim both ends into start/end indices over the original buffer.
    const char *start = callsign;
    while (*start == ' ')
        start++;
    const char *end = start + std::strlen(start);
    while (end > start && *(end - 1) == ' ')
        end--;

    size_t len = (size_t)(end - start);
    if (len == 0)
        return false; // empty after trim

    // Copy trimmed view into a fixed local buffer for downstream string checks.
    char buf[32];
    size_t n = len < sizeof(buf) - 1 ? len : sizeof(buf) - 1;
    std::memcpy(buf, start, n);
    buf[n] = '\0';

    // Airline flight (3 letters + digit) is never a tail number.
    char icao[4];
    if (parseAirlineIcao(buf, icao))
        return false;

    // Registration signals (any one is sufficient):
    //  * US N-number: leading 'N'/'n' followed by a digit.
    if ((buf[0] == 'N' || buf[0] == 'n') &&
        buf[1] >= '0' && buf[1] <= '9')
        return true;

    //  * hyphenated international registration (e.g. C-GABC, G-EUOA).
    if (std::strchr(buf, '-') != nullptr)
        return true;

    // Conservative: unknown / ambiguous is NOT auto-private.
    return false;
}

// isGeneralAviation: the GA/private side of the airline split, as ONE named
// predicate.
//
// "General aviation" here means exactly what Area mode's two-pass carousel has
// always meant by it: the callsign is not airline-format. That is deliberately
// BROADER than isTailNumber above -- a tail number is GA, but so is a
// non-airline callsign that is not registration-shaped either, and both belong
// on the same side of the split. Naming it once is what stops the two passes
// and the FlightWall-server path from drifting into different answers for the
// same aircraft, which is precisely what had happened: the server path applied
// hideCargo and the airline allow-list but never this rule, so a device set to
// that source showed GA traffic no matter how the setting was left.
//
// Takes the CALLSIGN, not a classified flight, so it stays usable at the point
// in Area mode where nothing has been enriched yet.
inline bool isGeneralAviation(const char *callsign)
{
    char icao[4];
    return !parseAirlineIcao(callsign, icao);
}

// isCargoOperator: membership in a curated freight-operator ICAO set
// (case-insensitive 3-letter compare).
inline bool isCargoOperator(const char *icao)
{
    if (!icao || icao[0] == '\0')
        return false;

    static const char *const kCargoIcao[] = {
        "FDX", "UPS", "GTI", "GEC", "CLX", "CKS", "CAO", "BOX", "ABW", "MPH",
        "PAC", "GSS", "NCA", "CKK", "SQC", "ABX", "CJT", "BCS", "DHK",
    };
    const size_t kCargoCount = sizeof(kCargoIcao) / sizeof(kCargoIcao[0]);

    // Uppercase the first three characters of the input.
    char up[4] = {0, 0, 0, 0};
    for (int i = 0; i < 3; ++i) {
        if (icao[i] == '\0')
            return false; // shorter than 3 chars -> cannot match any entry
        up[i] = (char)std::toupper((unsigned char)icao[i]);
    }

    for (size_t i = 0; i < kCargoCount; ++i) {
        if (up[0] == kCargoIcao[i][0] &&
            up[1] == kCargoIcao[i][1] &&
            up[2] == kCargoIcao[i][2])
            return true;
    }
    return false;
}

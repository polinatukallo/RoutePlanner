#include "parser.h"
#include <cstring>
#include <sstream>

Parameters parseArguments(int argc, char** argv) {
    Parameters params;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--from") == 0 && i + 1 < argc) {
            params.from = argv[++i];
        } else if (std::strcmp(argv[i], "--to") == 0 && i + 1 < argc) {
            params.to = argv[++i];
        } else if (std::strcmp(argv[i], "--date") == 0 && i + 1 < argc) {
            params.date = argv[++i];
        } else if (std::strcmp(argv[i], "--recommend") == 0) {
            params.recommendTrips = true;
        }
    }

    return params;
}

int convertToSeconds(const std::string& timeStr) {
    int hours = 0, minutes = 0;
    char colon;
    std::stringstream ss(timeStr);
    ss >> hours >> colon >> minutes;

    return (hours * 3600) + (minutes * 60);
}

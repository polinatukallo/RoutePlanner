#pragma once

#include <string>

// Struct to hold command-line arguments
struct Parameters {
    std::string from;
    std::string to;
    std::string date;
    bool recommendTrips = false;
};

// Parse arguments from the command line
Parameters parseArguments(int argc, char** argv);

// Convert HH:MM string to seconds
int convertToSeconds(const std::string& timeStr);

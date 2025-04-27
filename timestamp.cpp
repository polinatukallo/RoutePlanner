#include "timestamp.h"
#include <sstream>
#include <unordered_map>

int timeToTimestamp(const std::string& timeStr) {
    int hours = 0, minutes = 0;
    char colon;
    std::stringstream ss(timeStr);
    ss >> hours >> colon >> minutes;

    return (hours * 3600) + (minutes * 60);
}

std::string timestampToTime(int timestamp) {
    int hours = timestamp / 3600;
    int minutes = (timestamp % 3600) / 60;

    char buffer[6];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return std::string(buffer);
}

int getTimeZoneOffset(const std::string& city) {
    static const std::unordered_map<std::string, int> timeZones = {
        {"Moscow", 3 * 3600},
        {"London", 0},
        {"New_York", -5 * 3600},
        {"Tokyo", 9 * 3600},
        {"Paris", 1 * 3600}
        // Add more if needed
    };

    auto it = timeZones.find(city);
    if (it != timeZones.end()) {
        return it->second;
    }
    return 0; // Default to UTC if not found
}

int adjustTimestamp(int timestamp, int timeZoneOffset) {
    return timestamp + timeZoneOffset;
}

std::string invertDate(const std::string& dateStr) {
    // Input: "YYYY-MM-DD" -> Output: "DD-MM-YYYY"
    std::stringstream ss(dateStr);
    std::string year, month, day;

    std::getline(ss, year, '-');
    std::getline(ss, month, '-');
    std::getline(ss, day, '-');

    return day + "-" + month + "-" + year;
}

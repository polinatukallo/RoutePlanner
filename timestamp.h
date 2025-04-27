#pragma once

#include <string>

// Converts HH:MM time to timestamp in seconds
int timeToTimestamp(const std::string& timeStr);

// Converts timestamp in seconds to HH:MM string
std::string timestampToTime(int timestamp);

// Returns time zone shift in seconds (based on given city)
int getTimeZoneOffset(const std::string& city);

// Adjusts time by a timezone difference (positive or negative)
int adjustTimestamp(int timestamp, int timeZoneOffset);

// Converts date from YYYY-MM-DD to DD-MM-YYYY format
std::string invertDate(const std::string& dateStr);

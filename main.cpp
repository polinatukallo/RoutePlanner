#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <ctime>
#include <iomanip>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "timestamp.h"
#include "parser.h"

using namespace cpr;
using json = nlohmann::json;

// Class representing one trip segment
class TripSegment {
public:
    int arrivalTimeStamp;
    int departureTimeStamp;
    std::string departureCity;
    std::string arrivalCity;
    int timezoneOffset;

    bool hasTransfer = false;
    std::string transferCity = "no transfer";
    int transferArrival = 0;
    int transferDeparture = 0;

    // Print trip segment information
    void print() const {
        std::cout << "\n"
                  << "      FROM-TO:    " << departureCity << "  ->  " << arrivalCity << '\n'
                  << "      TIME:       " << timeInverter(stampToTime(departureTimeStamp)) 
                  << "      ->      " << timeInverter(stampToTime(arrivalTimeStamp)) << '\n'
                  << "      DATE:       " << dateInverter(stampToTime(departureTimeStamp)) 
                  << "    ->    " << dateInverter(stampToTime(arrivalTimeStamp)) << '\n'
                  << "      DURATION:   " << timeInverter(stampToTime(arrivalTimeStamp - departureTimeStamp - timezoneOffset)) << '\n';

        if (hasTransfer) {
            std::cout << "      TRANSFER:   Transfer at " << transferCity << "\n"
                      << "      DETAILS:    From " << timeInverter(stampToTime(transferArrival))
                      << " to " << timeInverter(stampToTime(transferDeparture)) << '\n';
        } else {
            std::cout << "      TRANSFER:   Direct trip (no transfer)\n";
        }
    }

    bool operator==(const TripSegment& other) const {
        return arrivalTimeStamp == other.arrivalTimeStamp &&
               departureTimeStamp == other.departureTimeStamp &&
               departureCity == other.departureCity &&
               arrivalCity == other.arrivalCity &&
               hasTransfer == other.hasTransfer;
    }
};

// Helper to get city code by name using Yandex Suggests API
std::string getCityCode(const std::string& cityName) {
    Response response = Get(Url{"https://suggests.rasp.yandex.net/all_suggests"}, 
                             Parameters{{"part", cityName}, {"format", "new"}});
    return json::parse(response.text)["items"][0]["point_key"];
}

// Request and process trip data
std::vector<TripSegment> fetchTrips(const Parameters& params) {
    const std::string cacheKey = params.from + "_" + params.to + "_" + params.date;
    const std::string cacheFile = "cache/" + cacheKey + ".json";

    std::vector<TripSegment> tripSegments;
    json responseJson;

    std::ifstream cacheStream(cacheFile);
    if (cacheStream.is_open()) {
        cacheStream >> responseJson;
        std::cout << "Loaded from cache.\n";
    } else {
        std::cout << "Starting HTTP request...\n";
        std::string requestUrl = "https://api.rasp.yandex.net/v3.0/search/?from=" + params.from + 
                                 "&to=" + params.to +
                                 "&format=json&lang=ru_RU&apikey=3afc2800-dd08-49b7-b90a-c037f1e1bdc1" +
                                 "&date=" + params.date +
                                 "&system=yandex&limit=100&transfers=true";

        Response response = Get(Url{requestUrl});
        if (response.text.empty()) {
            std::cerr << "Error: Empty server response.\n";
            return {};
        }

        if (response.status_code == 0) {
            std::cerr << response.error.message << '\n';
            return {};
        } else if (response.status_code >= 400) {
            std::cerr << "HTTP Error [" << response.status_code << "]\n";
            return {};
        } else if (response.status_code == 200) {
            responseJson = json::parse(response.text);
        }

        std::ofstream outFile(cacheFile);
        if (outFile.is_open()) {
            outFile << responseJson.dump(4);
        }
    }

    for (const auto& segment : responseJson["segments"]) {
        TripSegment trip;
        trip.arrivalTimeStamp = timeToStamp(segment["arrival"]);
        trip.departureTimeStamp = timeToStamp(segment["departure"]);
        trip.timezoneOffset = timeZone(segment["arrival"]) - timeZone(segment["departure"]);

        if (segment.contains("arrival_to")) {
            trip.departureCity = segment["departure_from"]["title"];
            trip.arrivalCity = segment["arrival_to"]["title"];
        } else {
            trip.departureCity = segment["from"]["title"];
            trip.arrivalCity = segment["to"]["title"];
        }

        trip.hasTransfer = segment["has_transfers"];
        if (trip.hasTransfer) {
            if (segment.contains("details") && segment["details"].is_array()) {
                trip.transferArrival = timeToStamp(segment["details"][0]["arrival"]);
                trip.transferDeparture = timeToStamp(segment["details"][2]["departure"]);
                trip.transferCity = segment["details"][0]["to"]["title"];
            }
        }

        tripSegments.push_back(trip);
    }

    return tripSegments;
}

// Find optimal trips there and back
void recommendTrips(const std::vector<TripSegment>& toTrips, const std::vector<TripSegment>& fromTrips) {
    std::vector<std::pair<TripSegment, TripSegment>> recommendations;
    std::pair<TripSegment, TripSegment> bestTrip;
    int minStayDuration;
    std::string input;

    std::cout << "Minimum desired stay in destination city? (Format: HH:MM)\n";
    std::cin >> input;
    minStayDuration = convertHours(input);

    for (int i = 0; i < 5; ++i) {
        int bestDuration = INT_MAX;

        for (const auto& to : toTrips) {
            for (const auto& from : fromTrips) {
                int stayDuration = from.departureTimeStamp - to.arrivalTimeStamp - to.timezoneOffset;
                if (stayDuration > minStayDuration && stayDuration < bestDuration &&
                    std::find(recommendations.begin(), recommendations.end(), std::make_pair(to, from)) == recommendations.end()) {
                    bestDuration = stayDuration;
                    bestTrip = std::make_pair(to, from);
                }
            }
        }

        recommendations.push_back(bestTrip);
    }

    std::cout << "\nRecommended trips:\n";
    int counter = 1;
    for (const auto& tripPair : recommendations) {
        std::cout << "**************************    PATH № " << counter++ << "   ***************************\n";
        tripPair.first.print();
        tripPair.second.print();
        std::cout << "\n\n";
    }
}

int main(int argc, char** argv) {
    Parameters params = parseArguments(argc, argv);

    params.from = getCityCode(params.from);
    params.to = getCityCode(params.to);

    std::vector<TripSegment> toTrips = fetchTrips(params);
    std::swap(params.from, params.to);
    std::vector<TripSegment> fromTrips = fetchTrips(params);

    if (params.recommendTrips) {
        recommendTrips(toTrips, fromTrips);
    } else {
        std::cout << "From-To:\n";
        for (const auto& trip : toTrips) {
            trip.print();
        }

        std::cout << "\n\nTo-From:\n";
        for (const auto& trip : fromTrips) {
            trip.print();
        }
    }

    return 0;
}

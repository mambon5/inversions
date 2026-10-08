#include "quote.hpp"
#include "dates.h"
#include "textProcess.h"
#include "time_utils.hpp"
#include "curl_utils.hpp"
using namespace std;
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


Quote::Quote(std::string symbol) {
    this->symbol = symbol;
}

Quote::~Quote() {}

size_t Quote::nbSpots() {
    return this->spots.size();
}

Spot Quote::getSpot(size_t i) {
    if (i < this->spots.size()) {
        return this->spots[i];
    }
    std::stringstream ss;
    ss << this->spots.size();

    std::string error = "ERROR: getSpot(index) - Index must be smaller than "
      + ss.str();
    throw std::invalid_argument(error);
}

Spot Quote::getSpot(std::time_t date) {
    for (std::vector<Spot>::iterator it = this->spots.begin();
         it != this->spots.end();
         ++it) {
        if (it->getDate() == date) {
            return *it;
        }
    }
    std::string error = "ERROR getSpot(date) - There is not spot at " + date;
    throw std::invalid_argument(error);
}

Spot Quote::getSpot(std::string date) {
    for (std::vector<Spot>::iterator it = this->spots.begin();
         it != this->spots.end();
         ++it) {
        if (it->getDateToString() == date) {
            return *it;
        }
    }
    std::string error = "ERROR getSpot(date) - There is not spot at " + date;
    throw std::invalid_argument(error);
}

void Quote::printSpots() {
    for (std::vector<Spot>::iterator it = this->spots.begin();
         it != this->spots.end();
         ++it) {
        std::cout << it->toString() << std::endl;
    }
}

vector<double> Quote::getCloseVals() {
    std::vector<double> closeValues;

    for (std::vector<Spot>::const_iterator it = this -> spots.begin();
         it != this -> spots.end();
         ++it) {
        closeValues.push_back(it->getCloseConst());
    }

    return closeValues;
}

vector<double> Quote::getVolumeVals() {
    std::vector<double> volumeValues;

    for (std::vector<Spot>::const_iterator it = this -> spots.begin();
         it != this -> spots.end();
         ++it) {
        volumeValues.push_back(it->getVolume());
    }

    return volumeValues;
}

void Quote::clearSpots() {
    this->spots.clear();
}

std::string Quote::getHistoricalJson(
    std::time_t period1,
    std::time_t period2,
    const char *interval
) {
    return downloadYahooJson(this->symbol, period1, period2, interval);
}

void Quote::getHistoricalSpotsJson(std::time_t period1,
                                   std::time_t period2,
                                   const char *interval) {
    std::string jsonStr = this->getHistoricalJson(period1, period2, interval);

    if (jsonStr.empty()) {
        std::cerr << "Sense dades rebudes per al ticker " << this->symbol << std::endl;
        return;
    }

    try {
        json j = json::parse(jsonStr, nullptr, false);

        if (j.is_discarded()) {
            std::cerr << "JSON invàlid rebut per a " << this->symbol << std::endl;
            return;
        }

        // Comprovar si Yahoo ha retornat un objecte d'error en el JSON
        if (j.contains("chart") && j["chart"].contains("error") && !j["chart"]["error"].is_null()) {
            std::string errCode = j["chart"]["error"]["code"];
            std::cerr << "Yahoo Error per a " << this->symbol << ": " << errCode << std::endl;
            return;
        }

        // Validar que existeixin els arrays principals
        if (!j.contains("chart") || !j["chart"].contains("result") || 
            j["chart"]["result"].is_null() || j["chart"]["result"].empty()) {
            return;
        }

        auto result = j["chart"]["result"][0];
        if (!result.contains("timestamp") || !result.contains("indicators")) {
            return;
        }

        auto timestamps = result["timestamp"];
        if (timestamps.is_null() || !result["indicators"].contains("quote") || result["indicators"]["quote"].empty()) {
            return;
        }

        auto quotes = result["indicators"]["quote"][0];

        std::vector<std::string> date, open, high, low, close, volume;

        for (size_t i = 0; i < timestamps.size(); ++i) {
            if (!timestamps[i].is_null()) {
                date.push_back(std::to_string(timestamps[i].get<long>()));
                open.push_back(quotes["open"][i].is_null() ? "0" : std::to_string(quotes["open"][i].get<double>()));
                high.push_back(quotes["high"][i].is_null() ? "0" : std::to_string(quotes["high"][i].get<double>()));
                low.push_back(quotes["low"][i].is_null() ? "0" : std::to_string(quotes["low"][i].get<double>()));
                close.push_back(quotes["close"][i].is_null() ? "0" : std::to_string(quotes["close"][i].get<double>()));
                volume.push_back(quotes["volume"][i].is_null() ? "0" : std::to_string(quotes["volume"][i].get<double>()));
            }
        }

        for (size_t i = 0; i < date.size(); ++i) {
            Spot spot = Spot(
                date[i],
                std::atof(open[i].c_str()),
                std::atof(high[i].c_str()),
                std::atof(low[i].c_str()),
                std::atof(close[i].c_str()),
                std::atof(volume[i].c_str())
            );
            this->spots.push_back(spot);
        }
    } catch (const std::exception& e) {
        std::cerr << "Excepció en processar " << this->symbol << ": " << e.what() << std::endl;
    }
}

void Quote::getHistoricalSpotsCsv(std::time_t period1,
                               std::time_t period2,
                               const char *interval) {
    // Download the historical prices Csv
    std::string csv = this->getHistoricalJson(period1, period2, interval);
    std::istringstream csvStream(csv);
    std::string line;

    // output the result
    // cout << "printing the curl result" << endl;
    // cout << csv << endl;

    // Remove the header line
    std::getline(csvStream, line);

    while (std::getline(csvStream, line)) {
        // Split line delimited by comma separator into a vector
        std::vector<std::string> spotVector;
        std::stringstream iss(line);
        std::string lineItem;
        while (std::getline(iss, lineItem, ',')) {
            spotVector.push_back(lineItem);
        }

        if (spotVector[0] != "null" && spotVector[1] != "null") {
            Spot spot = Spot(
                spotVector[0],                      // date
                std::atof(spotVector[1].c_str()),   // open
                std::atof(spotVector[2].c_str()),   // high
                std::atof(spotVector[3].c_str()),   // low
                std::atof(spotVector[4].c_str()),    // close
                std::atof(spotVector[6].c_str())    // volume (Yahoo CSV has Adj Close at 5, Volume at 6)
            );
            this->spots.push_back(spot);
        }
    }
}

void Quote::getHistoricalSpotsJson(const char *date1,
                               const char *date2,
                               const char *interval) {
    std::time_t period1 = dateToEpoch(date1);
    std::time_t period2 = dateToEpoch(date2);

    this->getHistoricalSpotsJson(period1, period2, interval);
}

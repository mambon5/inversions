#pragma once //fa que es compili només 1 vegada


using namespace std;
#include "quote.hpp"
#include "dates.h"
#include "textProcess.h"
#include "probab_utils.h"
#include "matrix_utils.h"

// #include "src/quote.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <string.h>

#include <dirent.h>

#include <istream>

// compile by the command: g++ get_dailies.cpp -o prova -lcurl
// or using: g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp get_dailies.cpp -o dailies -lcurl

double calcRSI14(const vector<double>& prices) {
    if (prices.size() < 15) return -1;

    double gain = 0, loss = 0;

    for (int i = prices.size() - 14; i < prices.size(); i++) {
        double delta = prices[i] - prices[i-1];

        if (delta > 0)
            gain += delta;
        else
            loss -= delta;
    }

    double avg_gain = gain / 14.0;
    double avg_loss = loss / 14.0;

    if (avg_loss == 0) return 100;
    if (avg_gain == 0) return 0;

    double rs = avg_gain / avg_loss;
    return 100 - (100 / (1 + rs));
}

double calcRelativeVolume30(const vector<double>& volumes) {
    if (volumes.size() < 31) return -1;

    double sum = 0;

    for (int i = volumes.size() - 31; i < volumes.size() - 1; i++) {
        sum += volumes[i];
    }

    double avg = sum / 30.0;
    double today = volumes.back();

    if (avg == 0) return -1;

    return today / avg;
}

void GetLastYearVals( std::string tick,const std::string & currentDate , double &slope, double &slope_6m, double &slope_1m, double &slope_6d, double &percentile, double &volatil, double &guanymax, double &perduamax, double &rsi14, double &relVol30, bool extraPrint=false, const std::string & initialDate = "2023-01-01") {
    //volatil vol dir la volatilitat de l'acció de borsa
    // std::string tick = "^GSPC";
    // S&P 500
    Quote *stock = new Quote(tick);
    // Get the historical spots from Yahoo Finance
    // pass std::strings to const char* vars
    const char * inid = initialDate.c_str();
    const char * currd = currentDate.c_str();    

    cout << "quote created from tick" << endl;

    stock->getHistoricalSpotsJson(inid, currd, "1d");   

    cout << "historial spots success" << endl;

    vector<double> closeVals;
    closeVals = stock->getCloseVals();
    cout << "historial spots success" << endl;

    // if(extraPrint) OutputVectorDouble(closeVals);

    if(extraPrint) cout << "Quantitat de valors: " << LenghtOfVectorDoub(closeVals) << endl; 
    if(size(closeVals) < 5) {
        cout << "returned" << endl;
        slope = 0;
        slope_6m = 0;
        slope_1m = 0;
        slope_6d = 0;
        percentile = 0;
        return;
    }

    vector<double> percent;
    percent = calcularPercentilVector(closeVals, closeVals);
    vector<int> roundedValues = roundToInteger(percent);
    int n = LenghtOfVectorInt(roundedValues);
    percentile = roundedValues[n-1];

    // Variables para almacenar los resultados de la regresión
    double intercept;

    // Calcular la regresión lineal
    vector<double> x;
    for (double i = 1; i <= LenghtOfVectorDoub(percent); ++i) {
        x.push_back(i);
    }
    volatil = volatilitat(closeVals);
    regression(x, percent, slope, intercept);

    // Calcular regressió per períodes: 6 mesos (aprox 126 dies), 1 mes (21 dies), 6 dies
    int n_all = percent.size();
    
    // 6 mesos
    int n6m = 126;
    if (n_all >= n6m) {
        vector<double> x6m, y6m;
        for (int i = 0; i < n6m; ++i) {
            x6m.push_back(i + 1);
            y6m.push_back(percent[n_all - n6m + i]);
        }
        regression(x6m, y6m, slope_6m, intercept);
    } else {
        slope_6m = slope; // default to annual if not enough data
    }

    // 1 mes
    int n1m = 21;
    if (n_all >= n1m) {
        vector<double> x1m, y1m;
        for (int i = 0; i < n1m; ++i) {
            x1m.push_back(i + 1);
            y1m.push_back(percent[n_all - n1m + i]);
        }
        regression(x1m, y1m, slope_1m, intercept);
    } else {
        slope_1m = slope;
    }

    // 6 dies
    int n6d = 6;
    if (n_all >= n6d) {
        vector<double> x6d, y6d;
        for (int i = 0; i < n6d; ++i) {
            x6d.push_back(i + 1);
            y6d.push_back(percent[n_all - n6d + i]);
        }
        regression(x6d, y6d, slope_6d, intercept);
    } else {
        slope_6d = slope;
    }

    // calcular guany màxim desde valor actual fins a màxim anual
    guanymax = guanymaxim(closeVals);
    perduamax = perduamaxima(closeVals);

    // RSI14 i RV30
    rsi14 = calcRSI14(closeVals);
    vector<double> volumeVals = stock->getVolumeVals();
    relVol30 = calcRelativeVolume30(volumeVals);

    // Free memory
    delete stock;
}

void PrintMainStocks(const vector<std::string> &ticks, std::string & file) {

    ofstream outputFile(file);
    bool printAll=false;
    std::string initialDate = "2023-01-01";

    // Date dt1 = { stoi(initialDate.substr(8,2)), 
    // stoi(initialDate.substr(5,2)), stoi(initialDate.substr(0,4)) }; 
    // Date dt2 = { stoi(currentDate.substr(8,2)), 
    // stoi(currentDate.substr(5,2)), stoi(currentDate.substr(0,4)) }; 
  
    // Function call 
    // int dies = calcularDiesEntreDates(dt1, dt2);

    std::string currentDate = getCurrentDate();

    // Write CSV Header
    outputFile << "Percentil,Slope 2y,Slope 6m,Slope 1m,Slope 6d,Volatilitat,Guany esperat,Perdua esperada,RSI14,RelVol30,Ticker" << endl;

    // cout << "Print all daily percentiles? Yes=1, No=0"<< endl;
    // cin >> printAll;
    
    for(std::string tick : ticks) {
        
         double slope, slope_6m, slope_1m, slope_6d, percent, volatil, guanyMax, perduaMax, rsi14, relVol30;

        GetLastYearVals(tick, currentDate, slope, slope_6m, slope_1m, slope_6d, percent,  volatil, guanyMax, perduaMax, rsi14, relVol30, true, initialDate);

        // Mostrar los resultados
        cout << percent << "," << PrintNumberWithXDecimalsDoub(slope,3) << "," << PrintNumberWithXDecimalsDoub(slope_6m,3) << "," << PrintNumberWithXDecimalsDoub(slope_1m,3) << "," << PrintNumberWithXDecimalsDoub(slope_6d,3) << "," << PrintNumberWithXDecimalsDoub(volatil,0) << ","
        << PrintNumberWithXDecimalsDoub(guanyMax,0) << "," << PrintNumberWithXDecimalsDoub(perduaMax,0) << "," << PrintNumberWithXDecimalsDoub(rsi14,1) << "," << PrintNumberWithXDecimalsDoub(relVol30,2) << "," << tick << endl;
        outputFile << percent << "," << PrintNumberWithXDecimalsDoub(slope,3) << "," << PrintNumberWithXDecimalsDoub(slope_6m,3) << "," << PrintNumberWithXDecimalsDoub(slope_1m,3) << "," << PrintNumberWithXDecimalsDoub(slope_6d,3) << "," << PrintNumberWithXDecimalsDoub(volatil,0) << "," 
        << PrintNumberWithXDecimalsDoub(guanyMax,0) << "," << PrintNumberWithXDecimalsDoub(perduaMax,0) << "," << PrintNumberWithXDecimalsDoub(rsi14,1) << "," << PrintNumberWithXDecimalsDoub(relVol30,2) << "," << tick << endl;


                    }

    outputFile.close();

}

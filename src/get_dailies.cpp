#include "quote.hpp"
#include "probab_utils.h"
#include "textProcess.h"
// #include "src/quote.hpp"
using namespace std;
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
// or using: g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp example.cpp -o dailies -lcurl

vector<string> ticks = { "^IXIC", "^GSPC", "^DJI", "META", "TSLA", "HPE", "PSX", "SU.PA", "RIOT",
"GOOGL", "GME", "INTC", "IBM", "QCOM", "ASML", "TSM", "NVDA", "005930.KS", "NFLX", "MSFT",
"LPL", "HLF", "AAPL",
"BTC-EUR", "ADA-EUR", "ETH-EUR", "EUR=X", "GC=F", "CL=F" };

void PrintLastYearVals(string tick, bool extraPrint=false) {
    string currentDate = getCurrentDate();
    cout << "Fecha actual: " << currentDate << endl;

    string initialDate = "2023-01-01";

    // string tick = "^GSPC";
    // S&P 500
    Quote *snp500 = new Quote(tick);

    // Get the historical spots from Yahoo Finance
    // pass strings to const char* vars
    const char * inid = initialDate.c_str();
    const char * currd = currentDate.c_str();    
    snp500->getHistoricalSpots(inid, currd, "1d");


    // printing just the close values
    vector<double> closeVals;

    closeVals = snp500->getCloseVals();
    // if(extraPrint) OutputVectorDouble(closeVals);

    // // Print the spots
    // snp500->printSpots();

    vector<double> percent;
    percent = calcularPercentilVector(closeVals, closeVals);
    vector<int> roundedValues = roundToInteger(percent);
    if(extraPrint) OutputVectorInt(roundedValues);
    int n = LenghtOfVectorInt(roundedValues);
    cout << "last close value percentile: " << roundedValues[n-1] << endl;
    

    // // Free memory
    // delete snp500;

}


void example_test() {
    // Ejemplo de uso
    vector<double> datos = {12.5, 30.2, 15.5, 3.2, 89.12, 14.2, 15.8, 18.3, 20.1, 22.7, 25.4, 28.0, 30.6, 33.2};

    // Valor para el cual se calculará el percentil
    double valor;
    cout << "enter value to compute percentile" << endl;
    cin >> valor;

    // Calcula el percentil
    double percentil = calcularPercentil(datos, valor);

    // Imprime el resultado
    cout << "El percentil del valor " << valor << " es: " << percentil << "%" << endl;
    cout << "percentils al complert:" << endl;

    vector<double> percent;
    percent = calcularPercentilVector(datos, datos);

    OutputVectorDouble(percent);






    string tick = "^GSPC";
    // S&P 500
    Quote *snp500 = new Quote(tick);

    // Get the historical spots from Yahoo Finance
    snp500->getHistoricalSpots("2023-10-01", "2023-12-31", "1d");

    cout << tick << endl;
    
    // Print the spots
    snp500->printSpots();


    // printing just the close values
    vector<double> closeVals;

    closeVals = snp500->getCloseVals();
    cout << "outputing el vector de closed values del ticker: " << tick << endl;
    OutputVectorDouble(closeVals);

    string currentDate = getCurrentDate();
    cout << "Fecha actual: " << currentDate << endl;


    // Print a spot
    try {
        Spot spot = snp500->getSpot("2023-12-18");
        cout << "Spot at 2023-12-18 closed at: " << endl;
        cout << spot.getClose() << endl;
    } catch(const exception &e) {
        cerr << e.what() << endl;
    }

    // Get the historical EUR/USD rates
    Quote *eurusd = new Quote("EURUSD=X");
    eurusd->getHistoricalSpots("2018-01-01", "2018-01-10", "1d");
    cout << "EUR/USD rates between 2018-01-01 and 2018-01-10" << endl;
    eurusd->printSpots();

    // Get the historical EUR/AUD rates
    Quote *euraud = new Quote("EURAUD=X");
    euraud->getHistoricalSpots("2018-01-01", "2018-01-10", "1d");
    cout << "EUR/AUD rates between 2018-01-01 and 2018-01-10" << endl;
    euraud->printSpots();

    // Free memory
    delete snp500;
    delete eurusd;
    delete euraud;
}



int main() {

    bool printAll;

    cout << "Print all daily percentiles? Yes=1, No=0"<< endl;

    cin >> printAll;


    for(string tick : ticks) {
        cout << endl << "#### " << tick << ": " << endl;
        PrintLastYearVals(tick, printAll);
    }

}
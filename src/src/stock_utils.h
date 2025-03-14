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


void GetLastYearVals( std::string tick,const std::string & currentDate , double &slope, double &percentile, double &volatil, double &guanymax, bool extraPrint=false, const std::string & initialDate = "2023-01-01") {
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

    // calcular guany màxim desde valor actual fins a màxim anual
    guanymax = guanymaxim(closeVals);

    // Free memory
    delete stock;
}

void PrintMainStocks(const vector<std::string> &ticks, std::string & file) {

    ofstream outputFile(file);
    bool printAll=false;
    std::string initialDate = "2023-01-01";

    cout << "initial date: " << initialDate << endl;
    outputFile << "initial date: " << initialDate << endl;

    std::string currentDate = getCurrentDate();
    cout << "Fecha actual: " << currentDate << endl;
    outputFile << "Fecha actual: " << currentDate << endl;

    // Date dt1 = { stoi(initialDate.substr(8,2)), 
    // stoi(initialDate.substr(5,2)), stoi(initialDate.substr(0,4)) }; 
    // Date dt2 = { stoi(currentDate.substr(8,2)), 
    // stoi(currentDate.substr(5,2)), stoi(currentDate.substr(0,4)) }; 
  
    // Function call 
    // int dies = calcularDiesEntreDates(dt1, dt2);
    int dies = DiesEntreDates(initialDate, currentDate);
    cout << "Quantitat de dies entre les dues dates: " << dies << endl; 
    outputFile << "Quantitat de dies entre les dues dates: " << dies << endl; 
    cout << "Quantitat de valors bursàtils analitzats: " << LenghtOfVectorStr(ticks) << endl;
    outputFile << "Quantitat de valors bursàtils analitzats: " << LenghtOfVectorStr(ticks) << endl;
    outputFile << "La volatilitat es calcula segons: (max-min)/max de l'acció en el període analitzat "  << endl;
    outputFile << "El pendent es calcula usant una regressió lineal "  << endl;
    outputFile << "El percentil és simplement el percentil actual respecte a la resta de valors el període analitzat "  << endl;
    cout << "Format dels resultats: percentil(%)  |  pendent anual  | volatilitat(%) | guany esperat(%) | etiqueta " << endl << endl;
    outputFile << "Format dels resultats: percentil(%)  |  pendent anual  | volatilitat(%) | guany esperat(%) | etiqueta "  << endl << endl;

    // cout << "Print all daily percentiles? Yes=1, No=0"<< endl;
    // cin >> printAll;
    
    for(std::string tick : ticks) {
        
        double slope, percent, volatil, guanyMax;

        GetLastYearVals(tick, currentDate, slope, percent,  volatil, guanyMax, true, initialDate);

        // Mostrar los resultados
        cout << percent << "  " << PrintNumberWithXDecimalsDoub(slope,3) << "  " << PrintNumberWithXDecimalsDoub(volatil,0) << "%  "
        << PrintNumberWithXDecimalsDoub(guanyMax,0) << "%  " << tick << endl;
        outputFile << percent << "  " << PrintNumberWithXDecimalsDoub(slope,3) << "  " << PrintNumberWithXDecimalsDoub(volatil,0) << "%  " 
        << PrintNumberWithXDecimalsDoub(guanyMax,0) << "%  " << tick << endl;


                    }

    outputFile.close();

}

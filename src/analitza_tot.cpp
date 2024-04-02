#include "stock_utils.h"
#include "matrix_utils.h"
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
#include <experimental/filesystem>

//There're some limitations by making the call to Yahoo Finance API: 
// Using the Public API (without authentication), you are limited to 2,000 requests per hour per IP 
// (or up to a total of 48,000 requests a day).

// compile by the command: g++ get_dailies.cpp -o prova -lcurl
// or using: g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp analitza_tot.cpp -o anal_main -lcurl

string dir = "/var/www/escolamatem/cpp/";
string slope_file = dir+"stocks_slope_percent.csv";
ofstream outfile("test.txt");
string lastTicker = dir+"lastTickerUsed.txt";
// ofstream lastTiFile(lastTicker);

// ofstream outputFile(slope_file);

string inp_file = "processed_ticks.csv";

vector<double> slope_parts {0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35};

void outputPercentSlope() {
    //  0. mira lultim ticker que s'ha analitzat
    //  1. Agafa els tickers que vinguin donats pel cin
    //  2. Tria una data inicial i final
    //  3. 
    int elems;

    cout << "mostrant intervals de classificació del pendent de les accions: " << endl;
    OutputVectorDouble(slope_parts);
    
    cout << "escriu el nombre de tickers que vols llegir:" << endl;
    cin >> elems;
    vector<string> tickers;
    // mira lultim ticker que s'ha analitzat:
    string lastTick;
    lastTick = GetFirstLineOfFile(lastTicker);
    // if last ticker exists, read after it
    if(lastTick != "") tickers = readPartialCsvFromCertainLine(inp_file, elems, lastTick);
    else tickers = readPartialCsv(inp_file, elems); // else, read the first tickers of the file

    ofstream outputFile;
    outputFile.open(slope_file, ios_base::app); // append instead of overwrite);
    bool printAll=false;
    string initialDate = "2023-01-01";

    cout << "initial date: " << initialDate << endl;
    outputFile << "initial date: " << initialDate << endl;

    string currentDate = getCurrentDate();
    cout << "Fecha actual: " << currentDate << endl;
    outputFile << "Fecha actual: " << currentDate << endl;
    vector<double> percents;
    double percent, slope;
    double index = 0;

    // tickers = {"0094.Z"};
    // tickers = {"0A22.IL"};
    outputFile.close();

    

    for(string tick : tickers) {
        cout << "analizing tickers... " << (index+1)/size(tickers)*100 << "%"<< endl;
        cout << endl << "#### " << tick << ": " << endl;
        GetLastYearVals(tick, currentDate, slope, percent,   true, initialDate);
        cout << "last year vals done" << endl;
        // cout << "slope: " << slope <<", percentile: " << percent << "%" << endl;
        // only print stocks with increasing trend in last year
        if(slope>0) {
            string line = to_string(percent) + "," + to_string(slope) + "," + tick;
            WriteToFileSimple(line, slope_file); // write results of pertile analysis
            WriteToFileOver(tick, lastTicker); // write tickername in  file
            cout << percent << "," << slope << "," << tick << endl;
        }
        index++;
    }
}

void showBestStocks() { //read from csv created in function outputPercentSlope()

    vector<double> percents;
    vector<vector<double>> slope_00, slope_01, slope_02, slope_03;
    double percent, slope;
    double index=0;
    vector<double> triplet = {percent, slope, index};
        if(slope >= 0 && slope < 0.1) {
            slope_00.push_back(triplet);
        } else if(slope >= 0.1 && slope < 0.2) {
            slope_01.push_back(triplet);
        } else if(slope >= 0.2 && slope < 0.3) {
            slope_02.push_back(triplet);
        } else if(slope >= 0.3) {
            slope_03.push_back(triplet);
        }

    cout << "showing best stocks: " << endl;
    cout << "size of slope_00: " << size(slope_00) << endl;
    cout << "size of slope_01: " << size(slope_01) << endl;
    cout << "size of slope_02: " << size(slope_02) << endl;
    cout << "size of slope_03: " << size(slope_03) << endl;

    int showFirst=5;

    cout << "showing elements of vector slope_00: " <<endl;
    vector<string> tickers;
    Output2Dvector_custom1(slope_00, tickers);
    vD_sortByCol(slope_00);

    cout << "showing sorted elements of vector slope_00: " <<endl;
    Output2Dvector_firstFew(slope_00, tickers, showFirst);

    vD_sortByCol(slope_01);
    cout << "showing sorted elements of vector slope_01: " <<endl;
    Output2Dvector_firstFew(slope_01, tickers, showFirst);

    vD_sortByCol(slope_02);
    cout << "showing sorted elements of vector slope_02: " <<endl;
    Output2Dvector_firstFew(slope_02, tickers, showFirst);

    vD_sortByCol(slope_03);
    cout << "showing sorted elements of vector slope_03: " <<endl;
    Output2Dvector_firstFew(slope_03, tickers, showFirst);
}


int main() {
    outputPercentSlope();
   
}
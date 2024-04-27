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

// compile by the command: 
// g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp analitza_tot.cpp -o anal_tot -lcurl

// if the computer makes more than 2000 requests per hour, you get an unauthorized error.
// sometimes it doesn't happend though.
// when it analyzes the last ticker, it starts over with the fist one again.

// to do:
//          1. (DONE) implement a function that deletes the files over 3 days old. To avoid ROM memory outage. function: DeleteOlderFiles()
//          2. (DONE) implement a function that ones it reaches the last ticker in "processed ticks" it starts again from the first one
//          3. Make a check function that looks if all tickers have already been processed for today, and halts current day processing if so.
string currentDate = getCurrentDate();

string dir = "/var/www/escolamatem/cpp/";
string slope_file_sufix ="stocks_slope_percent_"; // don't change this suffix without changing the function that deletes the old files of analysys
string slope_file =  dir+slope_file_sufix + currentDate+".csv";
ofstream outfile("test.txt");
string lastTicker = dir+"lastTickerUsed.txt";
// ofstream lastTiFile(lastTicker);

// ofstream outputFile(slope_file);

string inp_file = dir+"processed_ticks.csv";
// string inp_file = "mock_ticks.csv"; // this line is only for testing purposes

vector<double> slope_parts {0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35};


void outputPercentSlope(const int & elems) {
    //  0. mira lultim ticker que s'ha analitzat
    //  1. Agafa els tickers que vinguin donats pel cin
    //  2. Tria una data inicial i final
    //  3. 

    cout << "mostrant intervals de classificació del pendent de les accions: " << endl;
    OutputVectorDouble(slope_parts);
    
    cout << "escriu el nombre de tickers que vols llegir:" << endl;
    vector<string> tickers;
    // mira lultim ticker que s'ha analitzat:
    string lastTick;
    lastTick = GetFirstLineOfFile(lastTicker, true);
    // if last ticker exists, read after it
    if(lastTick != "") tickers = readPartialCsvFromCertainLine(inp_file, elems, lastTick);
    
    else if(!TodayFileExists(dir, slope_file_sufix)) tickers = readPartialCsv(inp_file, elems); // else, read the first tickers of the file
    // if todayfiles exists, and last tick is ="" it means the analysis of todays tickers is finished, so don't read more tickers

    ofstream outputFile;
    outputFile.open(slope_file, ios_base::app); // append instead of overwrite);
    bool printAll=false;
    string initialDate = "2023-01-01";

    cout << "initial date: " << initialDate << endl;
    outputFile << "initial date: " << initialDate << endl;

    
    cout << "Fecha actual: " << currentDate << endl;
    outputFile << "Fecha actual: " << currentDate << endl;
    vector<double> percents;
    double percent, slope;
    double index = 0;

    // tickers = {"0094.Z"};
    // tickers = {"0A22.IL"};
    outputFile.close();

    cout << "ticker length: " << LenghtOfVectorStr(tickers) << endl;
    cout << "elem length: " << elems << endl;

    for(string tick : tickers) {
        
        cout << endl << "#### " << tick << ": " << endl;
        cout << "analizing tickers... " << (index+1)/size(tickers)*100 << "%"<< endl;
        GetLastYearVals(tick, currentDate, slope, percent,   true, initialDate);
        cout << "last year vals done" << endl;
        // cout << "slope: " << slope <<", percentile: " << percent << "%" << endl;
        // only print stocks with increasing trend in last year
        if(slope>0) {
            
            string line = PrintNumberWithXDecimalsDoub(percent,0) + "," + 
                PrintNumberWithXDecimalsDoub(slope,3) + "," + tick;
            WriteToFileSimple(line, slope_file); // write results of pertile analysis
            cout << percent << "," << slope << "," << tick << endl;
        }
        WriteToFileOver(tick, lastTicker); // write tickername in  file
        index++;
    }
    cout << "after the loop:" << endl;
    cout << "ticker length: " << LenghtOfVectorStr(tickers) << endl;
    cout << "elem length: " << elems << endl;
    cout << "index length: " << index << endl;

    if(LenghtOfVectorStr(tickers) < elems) WriteToFileOver("", lastTicker); // reset last ticker to "" since we reached EOF (tickers read < we wanted to read)
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
    int elems = 2000;
    
    DeleteOlderFiles(dir, slope_file_sufix, 10, false); // deletes files with suffix "sufix" in directory "dir" which are "12" days old or older.
    outputPercentSlope(elems);
   
}
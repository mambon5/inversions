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
//          3. (DONE) Make a check function that looks if all tickers have already been processed for today, and halts current day processing if so.
string currentDate = getCurrentDate();
string dir = "/var/www/inversions/output/";
string slope_file_sufix ="stocks_slope_percent_"; // don't change this suffix without changing the function that deletes the old files of analysys
string slope_file =  dir+slope_file_sufix + currentDate+".csv";
ofstream outfile("test.txt");
string lastTicker = dir+"lastTickerUsed.txt";
// ofstream lastTiFile(lastTicker);
// ofstream outputFile(slope_file);

// string inp_file = "/var/www/inversions/input/processed_ticks.csv"; // this analyses all stock values yahoo finance has in the world (around 180k)
string inp_file = "/var/www/inversions/input/yfin_etoroTicks_clean.csv"; // this analyses all stock values eToro has and that we could properly translate to Yahoo finance ticks
// string inp_file = "mock_ticks.csv"; // this line is only for testing purposes

vector<double> slope_parts {0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35};


void outputPercentSlope(const int & elems) {
    //  0. mira lultim ticker que s'ha analitzat
    //  1. Agafa els tickers que vinguin donats pel cin
    //  2. Tria una data inicial i final
    //  3. 

    
    
    cout << "escriu el nombre de tickers que vols llegir:" << endl;
    vector<string> tickers;
    // mira lultim ticker que s'ha analitzat:
    string lastTick;
    lastTick = GetFirstLineOfFile(lastTicker, true);
    // if last ticker exists, read after it
    tickers = readPartialCsvFromCertainLine(inp_file, elems, lastTick);
    
    //if it is a new day, start from scratch!
    cout << "input file: " << inp_file << endl;
    if( !TodayFileExists(dir, slope_file_sufix)) tickers = readPartialCsv(inp_file, elems); 

    ofstream outputFile;
    outputFile.open(slope_file, ios_base::app); // append instead of overwrite);
    cout << "ticker first and second: " << tickers[0] << ", " << tickers[1] << endl;


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
        WriteToFileOver(tick, lastTicker); // write tickername in  file after analizing it
        index++;
    }
    cout << "after the loop:" << endl;
    cout << "ticker length: " << LenghtOfVectorStr(tickers) << endl;
    cout << "elem length: " << elems << endl;
    cout << "index length: " << index << endl;

    // if(LenghtOfVectorStr(tickers) < elems) WriteToFileOver("", lastTicker); // reset last ticker to "" since we reached EOF (tickers read < we wanted to read)
}


int main() {
    int elems = 2000;
    
    DeleteOlderFiles(dir, slope_file_sufix, 10, false); // deletes files with suffix "sufix" in directory "dir" which are "12" days old or older.
    outputPercentSlope(elems);
   
}
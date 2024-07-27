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

// g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp sort_stocks.cpp -o sort_stock_groups -lcurl


string date = yesterday();
string dir = "/var/www/inversions/output/";
string slope_file_sufix ="stocks_slope_percent_"; 
string inpFile =  dir + slope_file_sufix + date+".csv";
string stats_file_sufix ="stocks_sorted_stats_"; // don't change this suffix without changing the function that deletes the old files of analysys
string outFile = dir + stats_file_sufix + date + ".txt";


void showBestStocks(const string & inp_file) { //read from csv created in function outputPercentSlope()
// show the best performing stocks, selecting by minimal anual percentile, and max daily parameters from linear regression over 1 year time.

    vector<double> percents;
    vector<vector<double>> slope_00, slope_01, slope_02, slope_03;
    double percent, slope, volatil;
    vector<vector<string>> triplets = readCsvToMatrix(inp_file, 4); // quatre columnes per fila
    cout << "numero de files de la matriu: " << LenghtOfMatStr(triplets) << endl;
    cout << "numero de columnes de la matriu: " << LenghtOfVectorStr(triplets[0]) << endl;

    double index = 0;
    vector<string> tickers;
    for(vector<string> tripletString : triplets) {

        percent = stod(tripletString[0]);
        slope = stod(tripletString[1]);
        volatil = stod(tripletString[2]);
        tickers.push_back(tripletString[3]); // guardant els stock tickers

        vector<double> triplet = {percent, slope, volatil, index};

        if(slope >= 0 && slope < 0.1) {
            slope_00.push_back(triplet);
        } else if(slope >= 0.1 && slope < 0.2) {
            slope_01.push_back(triplet);
        } else if(slope >= 0.2 && slope < 0.3) {
            slope_02.push_back(triplet);
        } else if(slope >= 0.3) {
            slope_03.push_back(triplet);
        }
        index = index + 1;
    }

    WriteToFileSimple("showing best stocks: ",outFile);
    WriteToFileSimple("size of slope_00: " + to_string(size(slope_00)),outFile);
    WriteToFileSimple("size of slope_01: " + to_string(size(slope_01)),outFile);
    WriteToFileSimple("size of slope_02: " + to_string(size(slope_02)),outFile);
    WriteToFileSimple("size of slope_03: " + to_string(size(slope_03)),outFile);

    int showFirst=12;

    WriteToFileSimple("",outFile);
    WriteToFileSimple("Output format: ",outFile);
    WriteToFileSimple("percentile(%) - yearly slope - volatility(%) - yfin ticker ",outFile);
    WriteToFileSimple("",outFile);
    vD_sortBy2Col(slope_00);
    WriteToFileSimple("showing sorted elements of vector slope_00: ",outFile);
    Write2Dvector_firstFew(slope_00, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_01);
    WriteToFileSimple("showing sorted elements of vector slope_01: ",outFile);
    Write2Dvector_firstFew(slope_01, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_02);
    WriteToFileSimple("showing sorted elements of vector slope_02: ",outFile);
    Write2Dvector_firstFew(slope_02, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_03);
    WriteToFileSimple("showing sorted elements of vector slope_03: ",outFile);
    Write2Dvector_firstFew(slope_03, tickers, outFile, showFirst);
}

int main() {
      
    DeleteOlderFiles(dir, stats_file_sufix, 10, false); // deletes files with suffix "sufix" in directory "dir" which are "12" days old or older.
    showBestStocks(inpFile);
    
   

//    cout << "Introdueix una linia del csv: ";
//    int x;
//    while(cin >> x) {
//    cout << "Introdueix una columna del csv: ";
//    int y;
//    cin >> y;
//     cout << triplets[x][y] << endl;
//     cout << "Introdueix una linia del csv: ";
//     }
}
#include "stock_utils.h"
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

// g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp dates.cpp textProcess.cpp sort_stocks.cpp -o sort_stock_groups -lcurl


string date = today();
string dir = "/var/www/inversions/output/";
string slope_file_sufix ="stocks_slope_percent_"; 
string inpFile =  dir + slope_file_sufix + date+".csv";
string stats_file_sufix ="stocks_sorted_stats_"; // don't change this suffix without changing the function that deletes the old files of analysys
string outFile = dir + stats_file_sufix + date + ".txt";
#include <map>


void showBestStocks(const string & inp_file) {

    vector<double> percents;

    // ✅ nous grups negatius
    vector<vector<double>> slope_neg2, slope_neg1, slope_neg0;
    vector<vector<double>> slope_00, slope_01, slope_02, slope_03;

    double percent, slope, volatil, guanyMax, perduaMax;
    vector<vector<string>> triplets = readCsvToMatrix(inp_file, 6);

    cout << "numero de files de la matriu: " << LenghtOfMatStr(triplets) << endl;
    cout << "numero de columnes de la matriu: " << LenghtOfVectorStr(triplets[0]) << endl;

    double index = 0;
    vector<string> tickers;

    map<string, vector<double>> uniqueStocks;

    for(vector<string> tripletString : triplets) {
        percent = stod(tripletString[0]);
        slope = stod(tripletString[1]);
        volatil = stod(tripletString[2]);
        guanyMax = stod(tripletString[3]);
        perduaMax = stod(tripletString[4]);
        string ticker = tripletString[5];

        // ✅ filtre de dades buides (junk) o repetides
        if(percent == 0 && slope == 0) continue;
        
        uniqueStocks[ticker] = {percent, slope, volatil, guanyMax, perduaMax};
    }

    // Netejar tickers vells i reconstruir des del map per assegurar unicitat
    tickers.clear();
    index = 0;
    for(auto const& [ticker, valors] : uniqueStocks) {
        tickers.push_back(ticker);
        vector<double> triplet = {valors[0], valors[1], valors[2], valors[3], valors[4], index};

        // ✅ classificació nova (usant els mateixos rangs que a la visualització)
        if(triplet[1] < -0.2) {
            slope_neg2.push_back(triplet);
        } else if(triplet[1] >= -0.2 && triplet[1] < -0.1) {
            slope_neg1.push_back(triplet);
        } else if(triplet[1] >= -0.1 && triplet[1] < 0.0) {
            slope_neg0.push_back(triplet);
        } else if(triplet[1] >= 0 && triplet[1] < 0.1) {
            slope_00.push_back(triplet);
        } else if(triplet[1] >= 0.1 && triplet[1] < 0.2) {
            slope_01.push_back(triplet);
        } else if(triplet[1] >= 0.2 && triplet[1] < 0.3) {
            slope_02.push_back(triplet);
        } else if(triplet[1] >= 0.3) {
            slope_03.push_back(triplet);
        }
        index++;
    }

    WriteToFileOver("showing best stocks: ",outFile);

    // ✅ mides dels grups amb noms clars - ORDENATS DESCENDENT
    WriteToFileSimple("size slope >= 0.3: " + to_string(size(slope_03)),outFile);
    WriteToFileSimple("size 0.2 <= slope < 0.3: " + to_string(size(slope_02)),outFile);
    WriteToFileSimple("size 0.1 <= slope < 0.2: " + to_string(size(slope_01)),outFile);
    WriteToFileSimple("size 0.0 <= slope < 0.1: " + to_string(size(slope_00)),outFile);
    WriteToFileSimple("size -0.1 <= slope < 0.0: " + to_string(size(slope_neg0)),outFile);
    WriteToFileSimple("size -0.2 <= slope < -0.1: " + to_string(size(slope_neg1)),outFile);
    WriteToFileSimple("size slope < -0.2: " + to_string(size(slope_neg2)),outFile);

    int showFirst = 9999;

    WriteToFileSimple("",outFile);
    WriteToFileSimple("Output format: ",outFile);
    WriteToFileSimple("percentile(%) - yearly slope - volatility(%) - guanyMax(%) - perduaMax(%) - yfin ticker ",outFile);
    WriteToFileSimple("",outFile);

    // ✅ imprimir ORDENATS DESCENDENT
    vD_sortBy2Col(slope_03);
    WriteToFileSimple("showing slope >= 0.3: ",outFile);
    Write2Dvector_firstFew(slope_03, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_02);
    WriteToFileSimple("showing 0.2 <= slope < 0.3: ",outFile);
    Write2Dvector_firstFew(slope_02, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_01);
    WriteToFileSimple("showing 0.1 <= slope < 0.2: ",outFile);
    Write2Dvector_firstFew(slope_01, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_00);
    WriteToFileSimple("showing 0.0 <= slope < 0.1: ",outFile);
    Write2Dvector_firstFew(slope_00, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_neg0);
    WriteToFileSimple("showing -0.1 <= slope < 0: ",outFile);
    Write2Dvector_firstFew(slope_neg0, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_neg1);
    WriteToFileSimple("showing -0.2 <= slope < -0.1: ",outFile);
    Write2Dvector_firstFew(slope_neg1, tickers, outFile, showFirst);

    vD_sortBy2Col(slope_neg2);
    WriteToFileSimple("showing slope < -0.2: ",outFile);
    Write2Dvector_firstFew(slope_neg2, tickers, outFile, showFirst);
}



int main() {
      
    DeleteOlderFiles(dir, stats_file_sufix, 10, false); // deletes files with suffix "sufix" in directory "dir" which are "12" days old or older.
    cout << inpFile << endl;
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
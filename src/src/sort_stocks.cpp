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

void showBestStocks(const string & inp_file) { //read from csv created in function outputPercentSlope()
// show the best performing stocks, selecting by minimal anual percentile, and max daily parameters from linear regression over 1 year time.

    vector<double> percents;
    vector<vector<double>> slope_00, slope_01, slope_02, slope_03;
    double percent, slope;
    
    
    
    vector<vector<string>> triplets = readCsvToMatrix(inp_file, 3); // tres columnes per fila
    cout << "numero de files de la matriu: " << LenghtOfMatStr(triplets) << endl;
    cout << "numero de columnes de la matriu: " << LenghtOfVectorStr(triplets[0]) << endl;

    double index = 0;
    vector<string> tickers;
    for(vector<string> tripletString : triplets) {

        percent = stod(tripletString[0]);
        slope = stod(tripletString[1]);
        tickers.push_back(tripletString[2]); // guardant els stock tickers

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
        index = index + 1;
    }

    cout << "showing best stocks: " << endl;
    cout << "size of slope_00: " << size(slope_00) << endl;
    cout << "size of slope_01: " << size(slope_01) << endl;
    cout << "size of slope_02: " << size(slope_02) << endl;
    cout << "size of slope_03: " << size(slope_03) << endl;

    int showFirst=10;

    cout << "showing elements of vector slope_00: " <<endl;
    
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
  
  string filename = "stocks_slope_percent_2024-05-02.csv";
   showBestStocks(filename);
    
   

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

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

#include <dirent.h>

#include <istream>

// compile by the command: 
// g++ time_utils.cpp curl_utils.cpp quote.cpp spot.cpp dates.cpp textProcess.cpp analitza_main_stocks.cpp -o anal_main -lcurl

std::string file = "/var/www/inversions/output/pertiles.csv";

// vector<std::string> main_ticks = {"WFGDX"  };

vector<std::string> main_ticks = {"WFGDX" ,"^IXIC", "^GSPC", "^DJI", "META", 
"TSLA", "HPE", "PSX", "SU.PA", "RIOT", "SMCI",
"GOOGL", "GME", "INTC", "IBM", "AMZN","BABA","BRK-B","LLY","AVGO",
"WMT","JPM","XOM","V","QCOM", "ASML", "TSM", "NVDA", "005930.KS", 
"NFLX", "MSFT",
"LPL", "HLF", "AAPL", "0P0000XV9S.BO","2223.SR",
"BTC-EUR", "ADA-EUR", "ETH-EUR", "EUR=X", "GC=F", "CL=F",
 "CE","KRUS","ANF","DG3.BE","RDDT","VVVNF","CROX", "BAYN.DE","TTWO"};

int main() {
    PrintMainStocks(main_ticks, file);
}


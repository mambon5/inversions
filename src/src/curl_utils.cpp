#include "curl_utils.hpp"
#include "time_utils.hpp"

#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <cstdlib> // Per a exit()

// Variable per comptar quants 429/403 o timeouts consecutius tenim
static int consecutive_rate_limits = 0;

size_t writeCallback(char *content, size_t size, size_t nmemb, void *userdata) {
    // Append the content to user data
    ((std::string*)userdata)->append(content, size * nmemb);

    // Return the real content size
    return size * nmemb;
}

std::string downloadYahooJson(
    std::string symbol,
    std::time_t period1,
    std::time_t period2,
    std::string interval
) {
    std::stringstream ss1; 
    ss1 << period1; 
    std::stringstream ss2; 
    ss2 << period2;

    std::string url = "https://query2.finance.yahoo.com/v8/finance/chart/"
            + symbol
            + "?period1=" + ss1.str()
            + "&period2=" + ss2.str()
            + "&interval=" + interval
            + "&events=history";

    std::cout << url << std::endl;

    CURL* curl = curl_easy_init();
    std::string responseBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Write result into the buffer
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        // 1. TIMEOUTS CRÍTICS PER EVITAR PENJAMENTS
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8L); // Màxim 8s per establir connexió
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 12L);        // Màxim 12s per descarregar tot el fitxer

        // 2. USER-AGENT MODERN
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
        
        // 3. SEGUIR REDIRECCIONS AUTOMÀTIQUES
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // Cleanup
        curl_easy_cleanup(curl);

        // 4. GESTIÓ D'ERRORS I RATE LIMITS
        if (res != CURLE_OK) {
            std::cerr << "[Network Error] Timeout o fallida per a " << symbol << ": " << curl_easy_strerror(res) << std::endl;
            consecutive_rate_limits++;
        } else if (http_code == 429 || http_code == 403) {
            consecutive_rate_limits++;
            std::cerr << "[Rate Limit] HTTP " << http_code << " per a " << symbol 
                      << " (Errors consecutius: " << consecutive_rate_limits << ")" << std::endl;
        } else if (http_code == 200) {
            // Si la petició s'ha fet amb èxit, reiniciem el comptador d'errors
            consecutive_rate_limits = 0;
        }

        // Si tenim 3 o més bloquejos o timeouts consecutius, s'atura l'execució
        if (consecutive_rate_limits >= 3) {
            std::cerr << "S'ha assolit el límit de peticions o bloqueig temporal per part de Yahoo Finance. Aturant el programa per a la següent crida del cron..." << std::endl;
            exit(0);
        }
    }

    return responseBuffer;
}
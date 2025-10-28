#include "curl_utils.hpp"
#include "time_utils.hpp"

#include <curl/curl.h>
#include <sstream>
#include <iostream>

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
    // since septmeber 6th 2024, yahoo changed the format of the http query so now use:
    std::string url = "https://query2.finance.yahoo.com/v8/finance/chart/"
    //instead of the old:
    //std::string url = "https://query1.finance.yahoo.com/v7/finance/download/"
    // they changed "query1" --> "query2", "v7" for"v8" and "download" for "chart"
    // the outcome is in json instead of csv
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
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
        

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Cleanup
        curl_easy_cleanup(curl);
    }

    return responseBuffer;
}

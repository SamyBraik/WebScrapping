#include <iostream>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <vector>


CURL* curl; 
CURLcode res;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        // Gérer le problème de mémoire
        return 0;
    }
    return newLength;
}

std::string recuperationSpot(const std::string& ticker, const std::string interval, const std::string key){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string readBuffer;

    if (curl){
        std::string url = "https://api.twelvedata.com/time_series?symbol="+ticker+"&interval="+interval+"&apikey="+key;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    return readBuffer;        
}


std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

int main(){
    std::string key = "***";
    std::string ticker = "GOOG";
    std::string interval = "1day";
    std::string data = recuperationSpot(ticker,interval,key);
    std::vector splited = split(data,'{');
    for (auto i : splited) std::cout << i << std::endl;

    return 0;
}
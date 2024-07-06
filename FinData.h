#ifndef FinData
#define FinData

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
        return 0;
    }
    return newLength;
}

std::string fetchingData(const std::string spec,const std::string ticker, const std::string interval, const std::string key, const std::string start, const std::string end){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string readBuffer;

    if (curl){
        std::string url = "https://api.twelvedata.com/"+spec+"?symbol="+ticker+"&interval="+interval+"&start_date="+start+"&end_date"+end+"&apikey="+key;
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

std::string coreFetchingData(const std::string spec,const std::string ticker, const std::string key){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string readBuffer;

    if (curl){
        std::string url = "https://api.twelvedata.com/"+spec+"?symbol="+ticker+"&apikey="+key;
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

class Stock {
    private:
        const std::string ticker;
        const std::string key;
    public:
        Stock(std::string ticker,std::string key) : ticker(ticker), key(key) {}

        double spot(){
            std::string spec ="price";
            std::string data = coreFetchingData(spec,ticker,key);
            double price = std::stod(split(data,'"')[3]);

            return price;
        }

        std::vector<double> open(const std::string interval, const std::string start, const std::string end){
        std::string spec = "times_series";
        std::string data = fetchingData(spec,ticker,interval,key,start,end);
        std::vector<std::string> firstSplit = split(data,'{');
        std::vector<double> openPrice{};
        for (int i=0;i<=(firstSplit.size()-4);i++){
            std::vector<std::string> secondSplit = split(firstSplit[i+3],'"');
            openPrice.push_back(std::stod(secondSplit[7]));
        }
        openPrice.pop_back();
        return openPrice;
        }
    
        std::vector<double> close(const std::string interval, const std::string start, const std::string end){
            std::string spec = "times_series";
            std::string data = fetchingData(spec,ticker,interval,key,start,end);
            std::vector<std::string> firstSplit = split(data,'{');
            std::vector<double> closePrice{};
            for (int i=0;i<=(firstSplit.size()-4);i++){
                std::vector<std::string> secondSplit = split(firstSplit[i+3],'"');
                closePrice.push_back(std::stod(secondSplit[19]));
            }
            closePrice.pop_back();
            return closePrice;
        }

        std::string raw(const std::string interval, const std::string start, const std::string end){
            std::string spec = "times_series";
            std::string data = fetchingData(spec,ticker,interval,key,start,end);
            return data;
        
        }
};

#endif // FinData
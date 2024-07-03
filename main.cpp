#include <iostream>
#include <curl/curl.h>
#include <string>

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

void recuperationSpot(const std::string& ticker){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string readBuffer;

    if (curl){
        std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/"+ticker;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    std::cout << "Données reçues : " << readBuffer << std::endl;        
}


// void recuperationDonneesHistorique(const std::string& ticker){
//     curl_global_init(CURL_GLOBAL_DEFAULT);
//     curl = curl_easy_init();
//     std::string readBuffer;
//     if (curl){
//         curl_easy_setopt(curl, CURLOPT_URL, "https://fr.finance.yahoo.com/quote/"+ticker+"/history/");
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//         res = curl_easy_perform(curl);
//         if(res != CURLE_OK) {
//             std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//         }
//         curl_easy_cleanup(curl);
//     }
// }

int main(){
    std::string ticker = "GOOG";
    recuperationSpot(ticker);
    return 0;
}
#include <curl/curl.h>
#include <string.h>
#include <iostream>
#include <string>
#include <openssl/hmac.h>
#include <sstream>
#include <jsoncpp/json/json.h>
#include <unordered_map>
#include <iomanip>
#include <chrono>
#include "./root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
// #include <pthread.h>
#include <thread>
#include <vector>

#define MILLIS_IN_A_DAY 86400000
#define MILLIS_IN_AN_HOUR 3600000
#define MILLIS_IN_A_MINUTE 60000

class BAPI
{
private:
    /* data */
    CURL *curl;
    CURLcode res;
    std::string API_KEY;
    std::string API_SECRET;
    std::string websocket_host = "stream.binance.com";
    std::string websocket_port = "9443";
    const std::string baseUrl = "https://api.binance.com";
    std::string gs_strLastResponse;
    struct curl_slist *chunk = NULL;
    std::vector<pthread_t> tids;
    std::vector<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>*> active_websockets;

public:
    BAPI(/* args */);
    ~BAPI();
    std::unordered_map<std::string,std::string> parameters;
    void executeHTTPRequest();
    void sendSignedRequest(std::string httpMethod,std::string urlPath,std::string &response);
    // size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);
    void sendPublicRequest(std::string urlPath,std::string &response);
    std::string joinQueryParameters();
    std::string encryptWithHMAC(const char* key, const char* data);
    int64_t getTimeStamp();
    std::string getServerTime();
    bool orderFilled(std::string orderRes);
    std::string getFee(std::string symbol);
    std::string getSignature(std::string queryString);
    std::string getPrice(std::string symbol);
    std::string placeOrder(std::string symbol,std::string side,std::string type,float price,std::string quantity,std::string timeInForce);
    std::string placeOrder(std::string symbol,std::string side,std::string type,float price,std::string quantity,std::string timeInForce,float stopPrice);
    std::string placeOrder(std::string symbol,std::string side,std::string type,std::string quantity);
    std::string cancelOrder(std::string symbol,long long orderId);
    std::string cancelOrder(std::string symbol,long long orderId,std::string origClientOrderId);
    std::string getKlines(std::string symbol,std::string interval,int64_t startTime,int64_t endTime,int limit);
    std::string getKlines(std::string symbol,std::string interval,int64_t startTime,int64_t endTime);
    std::string getKlines(std::string symbol,std::string interval);
    std::string getExchangeInfo(std::string symbol);
    std::string getBook(std::string symbol);
    std::string getAccountInfo();
    void stream_reader(BAPI* myInstance,boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>* websock);
    void subscribeToWebsocket(std::string streamName);

    // std::string getLastResponse();
    



};



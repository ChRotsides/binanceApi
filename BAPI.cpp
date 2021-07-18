#include "BAPI.hpp"

struct package{
    char* data;
    size_t size;
};



size_t WriteCallback(void *ptr, size_t size, size_t count, void *data){


    size_t realsize=size*count;
    struct package *data_=(struct package *)data;    
    char *new_ptr=(char*)realloc(data_->data,data_->size + realsize + 1);
    
    data_->data=new_ptr;
    memcpy(&data_->data[data_->size],ptr,realsize);
    data_->size+=realsize;
    data_->data[data_->size]='\0';
    return realsize;
}
//SET BINANCE API KEY AND SECRET AS ENV VARIABLES
BAPI::BAPI(/* args */){

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl=curl_easy_init();
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    this->API_KEY=getenv("BINANCE_API_KEY");
    this->API_SECRET=getenv("BINANCE_API_SECRET");

    chunk = curl_slist_append(chunk, ("X-MBX-APIKEY:" + API_KEY).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);



}

BAPI::~BAPI(){
        
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
// size_t BAPI::WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {

//     this->gs_strLastResponse += (const char*)contents;
//     this->gs_strLastResponse += '\n';
//     return size * nmemb;
// }

std::string  BAPI::encryptWithHMAC(const char* key, const char* data) {
    unsigned char *result;
    static char res_hexstring[64];
    int result_len = 32;
    std::string signature;

    result = HMAC(EVP_sha256(), key, strlen((char *)key), const_cast<unsigned char *>(reinterpret_cast<const unsigned char*>(data)), strlen((char *)data), NULL, NULL);
  	for (int i = 0; i < result_len; i++) {
    	sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
  	}

  	for (int i = 0; i < 64; i++) {
  		signature += res_hexstring[i];
  	}

  	return signature;
}

std::string BAPI::getSignature(std::string queryString){
        std::string sign=encryptWithHMAC(API_SECRET.c_str(),queryString.c_str());
        return "&signature="+sign;


}

void BAPI::executeHTTPRequest() {
	CURLcode res;
	res = curl_easy_perform(curl);
	/* Check for errors */ 
	if(res != CURLE_OK)
	  fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
}

std::string BAPI::joinQueryParameters() {
	std::string queryString = "";
	for (auto it = parameters.cbegin(); it != parameters.cend(); ++it) {
			if (it == parameters.cbegin()) {
				queryString += it->first + '=' + it->second;
			}

			else {
				queryString += '&' + it->first + '=' + it->second;
			}
	}

	return queryString;
}
int64_t BAPI::getTimeStamp(){

        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

}
void BAPI::sendSignedRequest(std::string httpMethod,std::string urlPath,std::string &response){
    std::string url = baseUrl + urlPath + '?';
	std::string queryString = "";
    int64_t ms_epoch =getTimeStamp();
	std::string timeStamp = "timestamp=" + std::to_string(ms_epoch);

    //generate queryString
    if(!parameters.empty()){
        for(std::unordered_map<std::string,std::string>::const_iterator it=parameters.begin(); it!=parameters.end(); it++){
            if(it==parameters.begin()){
                queryString+=it->first+'='+it->second;
            }else {
				queryString += '&' + it->first + '=' + it->second;
			}
        }
        std::string res=queryString+'&'+timeStamp;
        url+=res;
        queryString.clear();
        queryString+=res;
    }else{
        url += timeStamp;
		queryString += timeStamp;
    }

    std::string signature=getSignature(queryString);
    url+=signature;
    queryString+=signature;
    // std::cout<<"url:"<<url<<"\n";

    struct package data;
    data.data=NULL;
    data.size=0;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    if(httpMethod=="POST"){
        curl_easy_setopt(curl,CURLOPT_HTTPGET,false);
        curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + urlPath).c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryString.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		parameters.clear();
		executeHTTPRequest();
    }else {
        curl_easy_setopt(curl,CURLOPT_HTTPGET,true);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		executeHTTPRequest();
	}

    response=data.data;

}
void BAPI::sendPublicRequest(std::string urlPath,std::string &response) {
    struct package data;
    data.data=NULL;
    data.size=0;
	std::string url = baseUrl + urlPath + '?';
    if (!parameters.empty()) {
    	url += joinQueryParameters();
    }
    // std::cout << "url:" << url << "\n";
    curl_easy_setopt(curl,CURLOPT_HTTPGET,true);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // std::cout<<url<<"\n";
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	parameters.clear();
	executeHTTPRequest();
    response=data.data;
}


std::string BAPI::getServerTime(){
    std::string response;
    sendPublicRequest("/api/v3/time",response);
    return response;
}

// /api/v3/ticker/price
std::string BAPI::placeOrder(std::string symbol,std::string side,std::string type,float price,std::string quantity,std::string timeInForce){
    parameters.clear();
    std::stringstream stream;
    stream.clear();
    stream<<std::setprecision(5)<<price;
    std::cout<<"Sent Price: "<<stream.str()<<"\n";

    parameters.insert({{"symbol",symbol},{"side",side},{"type",type},{"quantity",(quantity)},{"price",stream.str()},{"timeInForce",timeInForce}});
    std::string response;
    sendSignedRequest("POST","/api/v3/order",response);

    return response;
}

std::string BAPI::placeOrder(std::string symbol,std::string side,std::string type,std::string quantity){
    parameters.clear();

    parameters.insert({{"symbol",symbol},{"side",side},{"type",type},{"quantity",quantity}});
    std::string response;
    sendSignedRequest("POST","/api/v3/order",response);

    return response;
}

std::string BAPI::placeOrder(std::string symbol,std::string side,std::string type,float price,std::string quantity,std::string timeInForce,float stopPrice){
    parameters.clear();
    std::stringstream stream;
    stream.clear();
    stream<<std::setprecision(5)<<price;
    std::cout<<"Sent Price: "<<stream.str()<<"\n";

    parameters.insert({{"symbol",symbol},{"side",side},{"type",type},{"quantity",(quantity)},{"price",stream.str()},{"timeInForce",timeInForce},{"stopPrice",std::to_string(stopPrice)}});
    std::string response;
    sendSignedRequest("POST","/api/v3/order",response);

    return response;
}
std::string BAPI::cancelOrder(std::string symbol,long long orderId){
    parameters.clear();

    parameters.insert({{"symbol",symbol},{"orderId",std::to_string(orderId)}});
    std::string response;
    sendSignedRequest("DELETE","/api/v3/order",response);

    return response;
    }
std::string BAPI::cancelOrder(std::string symbol,long long orderId,std::string origClientOrderId){

    parameters.insert({{"symbol",symbol},{"orderId",std::to_string(orderId)},{"origClientOrderId",origClientOrderId}});
    std::string response;
    sendSignedRequest("DELETE","/api/v3/order",response);

    return response;
    }

std::string BAPI::getFee(std::string symbol){
    std::string response;
    parameters.clear();
    parameters.insert({"symbol",symbol});
    sendSignedRequest("GET","/sapi/v1/asset/tradeFee",response);

    return response;
}
std::string BAPI::getAccountInfo(){
    std::string response;
    parameters.clear();
    sendSignedRequest("GET","/api/v3/account",response);

    return response;
}
//Check if order is filled returns true if filled
bool BAPI::orderFilled(std::string orderRes){

    Json::Reader reader;
    Json::Value root;

    if(reader.parse(orderRes.c_str(),root)==false){
        return false;
    }
    return (root["status"].asString()=="FILLED");
}


std::string BAPI::getPrice(std::string symbol){


    std::string reqUrl;
    std::string response;
    parameters.clear();
    if(symbol==""){
       sendPublicRequest("/api/v3/ticker/price",response);
    }else{
        parameters.insert({"symbol",symbol});
        sendPublicRequest("/api/v3/ticker/price",response);
    }

    return response;
}
/*
Returns a json array of arrays:
    Open Time
    Open
    High
    Low
    Close
    Volume
    Close Time
    Quote asset volume
    Number of trades
    Taker buy base asset volume
    Taker buy quote asset volume
    Ignore
*/
std::string BAPI::getKlines(std::string symbol,std::string interval,int64_t startTime,int64_t endTime,int limit){
    std::string response;
    parameters.clear();

    parameters.insert({{"symbol",symbol},{"interval",interval},{"startTime",std::to_string(startTime)},{"endTime",std::to_string(endTime)},{"limit",std::to_string(limit)}});

    sendPublicRequest("/api/v3/klines",response);
    return response;
}
/*
Returns a json array of arrays:
    Open Time
    Open
    High
    Low
    Close
    Volume
    Close Time
    Quote asset volume
    Number of trades
    Taker buy base asset volume
    Taker buy quote asset volume
    Ignore
*/
std::string BAPI::getKlines(std::string symbol,std::string interval,int64_t startTime,int64_t endTime){
    std::string response;
    parameters.clear();

    parameters.insert({{"symbol",symbol},{"interval",interval},{"startTime",std::to_string(startTime)},{"endTime",std::to_string(endTime)}});

    sendPublicRequest("/api/v3/klines",response);
    return response;

}
/*
Returns a json array of arrays:
    Open Time
    Open
    High
    Low
    Close
    Volume
    Close Time
    Quote asset volume
    Number of trades
    Taker buy base asset volume
    Taker buy quote asset volume
    Ignore
*/
std::string BAPI::getKlines(std::string symbol,std::string interval){
    std::string response;
    parameters.clear();

    parameters.insert({{"symbol",symbol},{"interval",interval}});

    sendPublicRequest("/api/v3/klines",response);
    return response;
    
}

std::string BAPI::getExchangeInfo(std::string symbol){
    //https://api.binance.com/api/v3/exchangeInfo

    std::string response;
    parameters.clear();
    if(symbol!=""){
        parameters.insert({"symbol",symbol});
    }
    sendPublicRequest("/api/v3/exchangeInfo",response);
    return response;


}


std::string BAPI::getBook(std::string symbol){

    std::string response;
    parameters.clear();
    if(symbol!=""){
        parameters.insert({"symbol",symbol});
    }
    sendPublicRequest("/api/v3/ticker/bookTicker",response);
    return response;


}
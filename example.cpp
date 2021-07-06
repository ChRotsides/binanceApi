#include "global.hpp"
#include "BAPI.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>


int main(int argc,char *argv[]){

    BAPI bApi;
    std::unordered_map<std::string,std::string> parameters;

    std::string a=bApi.getServerTime();
    std::string b=bApi.getPrice("BTCEUR");

    Json::Value root;
    Json::Reader reader;


    // if(reader.parse(b.c_str(),root)==false){
    //     std::cerr<<"Unable to parse json\n"<<std::endl;
    //     exit(1);
    // }

    
    // std::cout<<stof(root["price"].asString())<<std::endl;
    // std::cout<<stof(root["price"].asString())+4<<std::endl;
    // std::string response;
    // bApi.sendSignedRequest("GET","/api/v3/account",response);

    // reader.parse(response,root);
    // std::cout<<response<<"\n";
    // std::cout<<root["balances"]<<"\n";
    // Json::Value balances=root["balances"];
    // for (Json::Value::const_iterator it=balances.begin(); it!=balances.end(); ++it) {
    //     std::cout << it.index() << std::endl;
    // }


    // for(int i=0; i<balances.size(); i++){

    //     Json::Value item= balances[i];
    //     if(i==0)
    //         for (Json::Value::const_iterator it=item.begin(); it!=item.end(); ++it) {
    //             std::cout << it.name() << std::endl;
    //             std::cout << it.key() << std::endl;
    //         }

    //     // std::cout<<item.asString()<<"\n";
    //     if(item["asset"].asString()=="EUR"){
    //         std::cout<<item["asset"].asString()<<"\n";
    //         std::cout<<item["free"].asString()<<"\n";

    //     }
    // }
    std::string adaPrice=bApi.getPrice("ADAEUR");
    std::cout<<"AdaString: "<<adaPrice<<"\n";
    Json::Value adaInfo;
    reader.parse(adaPrice.c_str(),adaInfo);

    std::string priceString = adaInfo["price"].asString();

    priceString.erase(remove(priceString.begin(), priceString.end(), '"'), priceString.end());

    float price=atof(priceString.c_str());

    std::cout<<"ADA Price:"<<price<<std::endl;
    float slippage=(1.0/100.0)*price;
    price-=slippage;
    // std::string OrderResponse=bApi.placeOrder("ADAEUR","SELL","LIMIT",price,10,"FOK");
    // std::cout<<"Order Res:"<<OrderResponse<<"\n";
    // std::cout<<"Order Filled:"<<bApi.orderFilled(OrderResponse)<<"\n";

    std::cout<<"ADA Fee: "<<bApi.getFee("ADAEUR")<<"\n";

    long long now=bApi.getTimeStamp();
    long long then=now-(long long)MILLIS_IN_A_DAY*(long long)10;
    // std::cout<<"klines: "<<bApi.getKlines("ADAEUR","1h",then,now,5);
    Json::Value klines;
    reader.parse(bApi.getKlines("ADAEUR","1d",then,now),klines);

    for(int i=0; i<klines.size(); i++){
        std::cout<<i<<" : "<<klines[i]<<"\n";
    }

    std::string adaInfo2=bApi.getExchangeInfo("ADAEUR");
    std::cout<<"AdaInfo: "<<adaInfo<<'\n';




    // std::cout<<a<<std::endl;
    // std::cout<<b<<std::endl;

    // dateTime now(15,6,2021,12,30,8);

    // std::cout<<"Time: "<<now.getEpochTime()<<std::endl;

    return 0;

}
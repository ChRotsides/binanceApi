#include "global.hpp"
#include "BAPI.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>
bool cont=1;

void exit_routine(){

    while(1){
        char c=getchar();
        if(c=='q'){
            cont=0;
            // break;
        }else if(c=='c'){
            cont=1;
            break;
        }
    }

}
int main(){

    BAPI bapi;


    auto wSoc=bapi.subscribeToWebsocket("btceur@bookTicker");
    boost::beast::flat_buffer buffer;
    std::thread tid(exit_routine);
    while(cont==1){
        wSoc->read(buffer);
        std::cout<<boost::beast::buffers_to_string(buffer.data())<<"\n";
        buffer.clear();
    }
    std::cout<<"End\n";
    bapi.endWebSocketConnection("btceur@bookTicker");
    tid.join();

    return 0;
}
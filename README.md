# Binance API Wrapper in C++
This repository contains a wrapper for the Binance API. It's written in C++ and provides a simple, straightforward way to interact with the Binance API. This includes functions for fetching server time, placing/cancelling orders, checking account info, checking order status, getting price info, getting historical kline/candlestick data, fetching exchange info, etc.

## Setup
To use this library, you need to set your Binance API key and secret as environment variables. This is done to secure your credentials:
``` export BINANCE_API_KEY="your_api_key_here" ```
``` export BINANCE_API_SECRET="your_api_secret_here" ```

## How to Use
Here is an example of how to use the BAPI class:
``` 
BAPI api;
std::string response = api.getServerTime(); 
```

## Methods
The `BAPI` class provides the following methods:

- getServerTime(): Fetches the server time from Binance.
- placeOrder(): Places an order. There are three versions of this function for different types of orders.
- cancelOrder(): Cancels a given order.
- getFee(): Fetches the trading fee for a given symbol.
- getAccountInfo(): Retrieves the account information.
- orderFilled(): Checks if a given order has been filled.
- getPrice(): Fetches the latest price for a given symbol.
- getKlines(): Fetches kline/candlestick data for a given symbol and interval.
- getExchangeInfo(): Retrieves exchange information for a given symbol.
- getBook(): Fetches the order book for a given symbol.


## Contributing
Contributions are welcome. Please submit a pull request, and make sure that your code adheres to the existing style.

## Disclaimer
Use this software at your own risk. The author is not responsible for any financial losses or disruptions of service.

## License
This project is licensed under the terms of the MIT license.

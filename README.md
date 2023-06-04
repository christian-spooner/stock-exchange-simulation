This project implements a stock exchange simulation using a microservices architecture. The system consists of four key services
- Order Entry Gateway
- Matching Engine
- Clearinghouse
- Market Data Feed
The services communicate asynchronously by sending and receiving messages through a RabbitMQ broker.

The Order Entry Gateway receives order requests, converts them into sequenced orders, and forwards them to the Matching Engine. 

The Matching Engine receives orders, matches them against an order book, and sends execution requests for matched orders to the Clearinghouse. 

The Clearinghouse receives execution requests, clears and settles both sides of the trade, and sends settlement reports to the Market Data Feed. 

The Market Data Feed receives and stores data from all services and provides an API for accessing the data.

To simulate market participants, the project includes a 'trading-bots' directory containing Python-based traders. A trader submits orders to the gateway according to a particular strategy (e.g. a random walk). The Market Data Feed then offers endpoints for retrieving data related to each trader. For instance, you can access the current share holdings of a trader, for a given security, using the endpoint "/holdings/{securityId}/{clientId}". Each trader also has a current account balance, initially set to $10,000 and varying based on their trading activity. This balance can be accessed from "/balance/{clientId}". This means that trading bots can be tested against each other and evaluated according to their final share holdings and account balances.
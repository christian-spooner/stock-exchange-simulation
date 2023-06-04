This project implements a stock exchange simulation using a microservices architecture. The system consists of four key services
- Order Entry Gateway
- Matching Engine
- Clearinghouse
- Market Data Feed

The Order Entry Gateway receives order requests, converts them into sequenced orders, and forwards them to the Matching Engine. 

The Matching Engine receives orders, matches them against an order book, and sends execution requests for matched orders to the Clearinghouse. 

The Clearinghouse receives execution requests, clears and settles both sides of trade, and sends a settlement report to the Market Data Feed. 

The Market Data Feed receives and stores data from all services and provides an API for accessing the data.

To simulate market participants, the project includes a 'trading-bots' directory containing Python-based traders. Each trader places orders to the gateway based on a particular strategy (e.g. a random walk). The Market Data Feed offers endpoints for retrieving a trader's current share holdings based on their 'client_id' and 'security_id'. Additionally, there is an endpoint to access a trader's current account balance in dollars. Initially set at $10,000 for every trader, the account balance varies according to their trading activities.
# Stock Exchange Simulation

This project implements a **Stock Exchange Simulation** using a **microservices architecture**. The system is built around four key services:

1. **Order Entry Gateway**
2. **Matching Engine**
3. **Clearinghouse**
4. **Market Data Feed**

These services communicate asynchronously by sending and receiving messages through a RabbitMQ broker.

## System Overview

**Order Entry Gateway**

Receives order requests, converts them into sequenced orders, and forwards them to the Matching Engine.

**Matching Engine**

Receives orders, matches them against an order book, and sends execution requests for matched orders to the Clearinghouse.

**Clearinghouse**

Receives execution requests, clears and settles both sides of the trade, and sends settlement reports to the Market Data Feed.

**Market Data Feed**

Receives and stores data from all services and provides an API for accessing the data.

**Trading Bots**

To simulate market participants, the project includes a 'trading-bots' directory containing Python-based traders. A trader submits orders to the gateway according to a particular strategy (e.g., a random walk).

The Market Data Feed offers endpoints for retrieving data related to each trader. E.g.

- Current share holdings of a trader, for a given security: `/holdings/{securityId}/{clientId}`
- Current account balance of a trader: `/balance/{clientId}`

Each trader starts with an initial balance of $10,000, which varies based on their trading activity. Hence, trading bots can be tested against each other and evaluated according to their final share holdings and account balances.

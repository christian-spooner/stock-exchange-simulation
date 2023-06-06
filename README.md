# Stock Exchange Simulation

This project implements a stock exchange simulation using a microservices architecture. To build and deploy the exchange locally, run
```
docker compose build
docker compose up
```

## System Overview

### Core Services

- `order-entry-gateway` 
    - Recieves order requests
    - Converts valid order requests into sequenced orders
    - Sends orders to `matching-engine`
    - Sends order reports to `market-data-feed`

- `matching-engine`
    - Maintains a separate, in-memory order book for each security listed on the exchange 
    - Recieves orders and matches them against the corresponding order book
    - Sends execution requests for matched orders to `clearinghouse`
    - Sends trade reports and price updates to `market-data-feed`

- `clearinghouse`
    - Receives execution requests
    - Clears and settles valid execution requests
    - Sends settlement reports to `market-data-feed`

- `market-data-feed`
    - Recieves and stores data received from every other service
    - Provides an API for data access that includes both REST and WebSocket endpoints

### Additional Components

- Persistence

    The services share a Postgres server with two databases: `clearing_data` and `market_data`.

    - `clearing_data` maintains a record of account balances for traders as well as a securities deposit to track share holdings
    - `market_data` stores reports from other services, as well as price updates from `matching-engine`

- Inter-service communication

    Services communicate via a RabbitMQ server. The server contains a *direct* exchange named `stock_exchange` that all messages are published to. This exchange maintains three *durable* queues to which messages are routed to and consumed from: 
    - `order_entry`
    - `order_execution`
    - `data_feed`

- Market participants

   To simulate market participants, the project includes the `trading-bots` directory containing Python-based traders. A trader submits orders to the gateway according to a particular strategy (e.g., a random walk).

   `market-data-feed` offers endpoints for retrieving data related to each trader, such as:

   - `/holdings/{securityId}/{clientId}`: current share holdings of a trader, for a given security
   - `/balance/{clientId}`: current account balance of a trader

   Each trader starts with an initial balance of $10,000, which varies based on their trading activity. Hence, trading bots can be tested against each other and evaluated according to their final share holdings and account balances.
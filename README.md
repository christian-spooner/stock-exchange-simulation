Services:
- Order Entry Gateway
- Matching Engine
- Clearinghouse
- Market Data Feed

Order Entry Gateway: receives order requests, transforms them into sequenced orders, sends orders to the matching engine.

Matching Engine: receives orders, matches them against order book, sends execution requests for matched orders to the clearinghouse.

Clearinghouse: receives execution requests for matched orders, clears and settles each trade, sends settlement report to market data feed.

Market data feed: receives and stores data from each other service, provides API for data access.



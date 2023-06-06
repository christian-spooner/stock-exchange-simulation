import asyncio
import json
from collections import deque

import requests
import websockets

from constants import *

BID = 0
ASK = 0
SPREAD = 0
IDS = deque()


def update_prices(bid, ask):
    global BID, ASK, SPREAD
    BID = bid
    ASK = ask
    SPREAD = round(ask - bid, 2)
    print(BID, ASK, SPREAD)


def init_prices():
    response = requests.get("http://localhost:2404/MarketData/price/MSFT")
    data = response.json()
    update_prices(data["bid"], data["ask"])


async def price_stream():
    try:
        uri = "ws://localhost:2404/stream/price"
        async with websockets.connect(uri) as websocket:
            while True:
                price_data = await websocket.recv()
                price_data = json.loads(price_data)
                update_prices(price_data["bid"], price_data["ask"])
    except KeyboardInterrupt:
        raise
    except Exception as err:
        print(err)
        raise


async def send_to_gateway(message: dict):
    global IDS
    serialised_message = json.dumps(message)
    reader, writer = await asyncio.open_connection(HOST, PORT)
    writer.write(serialised_message.encode("utf-8"))
    await writer.drain()
    response = await reader.read(100)
    response_data = json.loads(response.decode())
    print(f"Received: {response_data}")

    writer.close()
    await writer.wait_closed()

    IDS.append(response_data["order_id"])


async def send_cancel_to_gateway(*args):
    for id in args:
        message = {
            "type": "cancel",
            "order_id": id,
            "security_id": "MSFT",
        }
        serialised_message = json.dumps(message)
        reader, writer = await asyncio.open_connection(HOST, PORT)
        writer.write(serialised_message.encode("utf-8"))
        await writer.drain()
        response = await reader.read(100)
        response_data = json.loads(response.decode())
        print(f"Received: {response_data}")

    writer.close()
    await writer.wait_closed()


async def make_market():
    try:
        global BID, ASK, SPREAD, IDS
        while True:
            if SPREAD > 0.1:
                bid_price = round(BID + SPREAD * 0.1, 2)
                ask_price = round(ASK - SPREAD * 0.1, 2)

                bid_order = {
                    "type": "limit",
                    "security_id": "MSFT",
                    "client_id": "MARKET_MAKER",
                    "side": "buy",
                    "quantity": 2,
                    "price": bid_price,
                }
                await send_to_gateway(bid_order)

                ask_order = {
                    "type": "limit",
                    "security_id": "MSFT",
                    "client_id": "MARKET_MAKER",
                    "side": "sell",
                    "quantity": 2,
                    "price": ask_price,
                }
                await send_to_gateway(ask_order)

            if len(IDS) > 10:
                id1 = IDS.popleft()
                id2 = IDS.popleft()
                await send_cancel_to_gateway(id1, id2)

            # await asyncio.sleep(5)
    except KeyboardInterrupt:
        raise
    except Exception as err:
        print(err)
        raise


async def main():
    init_prices()
    tasks = [price_stream(), make_market()]
    await asyncio.gather(*tasks)


if __name__ == "__main__":
    asyncio.run(main())

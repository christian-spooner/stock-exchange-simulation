import asyncio

import websockets


async def price_stream():
    uri = "ws://localhost:2404/stream/price"
    async with websockets.connect(uri) as websocket:
        while True:
            price_data = await websocket.recv()
            print(price_data)


asyncio.run(price_stream())

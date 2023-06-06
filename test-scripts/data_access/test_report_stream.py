import asyncio

import websockets


async def report_stream():
    uri = "ws://localhost:2404/stream/report"
    async with websockets.connect(uri) as websocket:
        while True:
            report_data = await websocket.recv()
            print(report_data)


asyncio.run(report_stream())

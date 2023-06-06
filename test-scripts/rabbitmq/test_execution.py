import json
import random
import time

import pika


def generate_trade_execution():
    return {
        "order_id": 10,
        "client_id": "TEST",
        "security_id": "AAPL",
        "side": "buy",
        "price": 100.12,
        "quantity": 1,
    }


def send_trade_execution(channel, trade_execution):
    message = json.dumps(trade_execution)
    channel.basic_publish(
        exchange="",
        routing_key="order_execution",
        body=message,
        properties=pika.BasicProperties(delivery_mode=2),
    )
    print("Sent message:", message)


def main():
    credentials = pika.PlainCredentials("guest", "guest")
    parameters = pika.ConnectionParameters("localhost", 5700, "/", credentials)
    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()
    channel.queue_declare(queue="order_execution", durable=True)
    trade_execution = generate_trade_execution()
    send_trade_execution(channel, trade_execution)


if __name__ == "__main__":
    main()

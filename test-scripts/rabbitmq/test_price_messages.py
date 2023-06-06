import json
import random

import pika

# import time


def send_report_messages():
    credentials = pika.PlainCredentials("guest", "guest")
    parameters = pika.ConnectionParameters("localhost", 5700, "/", credentials)
    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()
    channel.queue_declare(queue="data_feed", durable=True)

    body = {"ask": 10049, "bid": 9983, "security_id": "MSFT", "timestamp": "1685104715"}
    message = json.dumps({"message_type": "price", "body": body})
    channel.basic_publish(
        exchange="financial_exchange",
        routing_key="data_feed",
        body=message,
        properties=pika.BasicProperties(delivery_mode=2),
    )
    print(f"Sent message: {message}")

    connection.close()


if __name__ == "__main__":
    send_report_messages()

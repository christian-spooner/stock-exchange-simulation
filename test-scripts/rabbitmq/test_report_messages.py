import json
import random

import pika


def send_report_messages():
    credentials = pika.PlainCredentials("guest", "guest")
    parameters = pika.ConnectionParameters("localhost", 5700, "/", credentials)
    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()
    channel.queue_declare(queue="data_feed", durable=True)

    report = {
        "event_type": "order",
        "status": "confirmed",
        "order_id": 12,
        "client_id": "ABC",
        "security_id": "MSFT",
        "side": "buy",
        "quantity": 10,
        "price": 50,
    }
    message = json.dumps({"message_type": "report", "body": report})
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

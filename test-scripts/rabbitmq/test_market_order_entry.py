import json

import pika


def send_order(order):
    credentials = pika.PlainCredentials("guest", "guest")
    parameters = pika.ConnectionParameters("localhost", 5700, "/", credentials)
    connection = pika.BlockingConnection(parameters)
    channel = connection.channel()
    channel.queue_declare(queue="data_feed", durable=True)
    message = json.dumps(order)
    channel.basic_publish(
        exchange="financial_exchange",
        routing_key="order_entry",
        body=message,
        properties=pika.BasicProperties(delivery_mode=2),
    )
    print(f"Sent message: {message}")

    connection.close()


if __name__ == "__main__":
    order = {
        "type": "limit",
        "order_id": 1,
        "security_id": "MSFT",
        "client_id": "JOHN",
        "side": "buy",
        "quantity": 100,
        "price": 14550,
    }

    order2 = {
        "type": "market",
        "order_id": 2,
        "security_id": "MSFT",
        "client_id": "JIM",
        "side": "sell",
        "quantity": 100,
        "price": 0,
    }

    send_order(order)
    send_order(order2)

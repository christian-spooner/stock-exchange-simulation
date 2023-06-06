import json

import pika

credentials = pika.PlainCredentials("guest", "guest")
parameters = pika.ConnectionParameters("localhost", 5700, "/", credentials)
connection = pika.BlockingConnection(parameters)
channel = connection.channel()

queue_names = ["data_feed", "order_entry", "order_execution"]
for q in queue_names:
    channel.queue_declare(queue=q, durable=True)
    message_count = channel.queue_purge(queue=q)
    print(f"Cleared {message_count.method.message_count} messages from queue: {q}")

connection.close()

import json
import socket

order = {
    "type": "limit",
    "security_id": "MSFT",
    "client_id": "JIM",
    "side": "sell",
    "quantity": 200,
    "price": 155.1263483,
}

message = json.dumps(order)

HOST = "127.0.0.1"
PORT = 2400
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

client_socket.sendall(message.encode("utf-8"))

response = client_socket.recv(1024)
print(f"Received response: {response}")

client_socket.close()

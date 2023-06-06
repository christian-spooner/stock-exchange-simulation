import json
import socket

cancel = {
    "type": "cancel",
    "order_id": 1,
    "security_id": "MSFT",
}

message = json.dumps(cancel)

HOST = "127.0.0.1"
PORT = 2400
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

client_socket.sendall(message.encode("utf-8"))

response = client_socket.recv(1024)
print(f"Received response: {response}")

client_socket.close()

import json
import socket

from constants import *


def send_to_gateway(message: dict):
    serialised_message = json.dumps(message)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((HOST, PORT))
    client_socket.sendall(serialised_message.encode("utf-8"))
    response = client_socket.recv(1024)
    print(f"Received response: {response}")
    client_socket.close()

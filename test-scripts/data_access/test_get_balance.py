import json
import socket
import time


def send_settlement_command(command):
    host = "127.0.0.1"
    port = 2403
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_sock:
        client_sock.connect((host, port))
        message = json.dumps(command)
        message_bytes = message.encode()

        message_length = len(message_bytes)
        length_buffer = message_length.to_bytes(4, "little")
        client_sock.sendall(length_buffer)

        client_sock.sendall(message_bytes)
        balance = client_sock.recv(1024)
        print(int.from_bytes(balance, byteorder="little"))


if __name__ == "__main__":
    send_settlement_command({"type": "balance", "client_id": "1"})

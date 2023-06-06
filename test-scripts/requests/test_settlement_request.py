import json
import socket


def send_settlement_request(message):
    host = "127.0.0.1"
    port = 2403
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_sock:
        client_sock.connect((host, port))
        message = json.dumps(message)
        message_bytes = message.encode()

        message_length = len(message_bytes)
        length_buffer = message_length.to_bytes(4, "little")
        client_sock.sendall(length_buffer)

        client_sock.sendall(message_bytes)
        balance = client_sock.recv(1024)
        print(balance)


if __name__ == "__main__":
    send_settlement_request({"type": "deposit", "client_id": "1", "amount": 100.12})
    send_settlement_request({"type": "withdraw", "client_id": "1", "amount": 50.24})
    send_settlement_request(
        {"type": "transfer", "client_id": "1", "to_client_id": "2", "amount": 25}
    )

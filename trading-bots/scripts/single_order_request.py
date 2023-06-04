from network import *


def main():
    order = {
        "type": "limit",
        "security_id": DEFAULT_SECURITY_ID,
        "client_id": "SINGLE",
        "side": "sell",
        "quantity": 20,
        "price": 100.12,
    }

    send_to_gateway(order)


if __name__ == "__main__":
    main()

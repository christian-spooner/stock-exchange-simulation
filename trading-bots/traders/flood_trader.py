import random

from network import *


def main():
    while True:
        side = random.randint(0, 1)
        if side == 0:
            side = "buy"
        else:
            side = "sell"
        order = {
            "type": "limit",
            "security_id": DEFAULT_SECURITY_ID,
            "client_id": "FLOOD",
            "side": side,
            "quantity": 10,
            "price": MEAN_PRICE,
        }

        send_to_gateway(order)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

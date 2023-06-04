import random
import time

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
            "client_id": "GAUSS",
            "side": side,
            "quantity": 4,
            "price": random.gauss(MEAN_PRICE, 10),
        }

        send_to_gateway(order)
        time.sleep(5)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
